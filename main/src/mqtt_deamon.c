#include <mqtt_deamon.h>

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <driver/gptimer.h>
/* SIM800-specyfic libraries */
#include <SIM.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
/* MbedTLS & ESP32 specyfic libraries */
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
// #include <mbedtls_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/esp_debug.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include <psa/crypto.h>
#endif
#include <esp_crt_bundle.h>
/* Main deamon */
#include <error_handler.h>
#include <cert.h>
/* UART deamon */
#include <uart_deamon.h>
#include <cmd_defs.h>
/**/
#include <info_ch.h>
#include <set_ch.h>

#define TAG "MQTT_DEAMON"

extern SIM_intf *sim;
extern struct mqtt_client client;
static QueueHandle_t cts_queue = NULL;

// static SemaphoreHandle_t mutex;
// static StaticSemaphore_t xMutexBuffer;
static void IRAM_ATTR cts_handle(void *arg)
{
    ESP_DRAM_LOGI(TAG, "CTS INTERRUPT");

    int intr_type = (uint32_t)arg;

    if (cts_queue != NULL)
    {
        xQueueSendFromISR(cts_queue, &intr_type, NULL);
    }
}

static void IRAM_ATTR rts_handle(void *arg)
{
    ESP_DRAM_LOGI(TAG, "RTS INTERRUPT");

    int intr_type = (uint32_t)arg;

    if (cts_queue != NULL)
    {
        xQueueSendFromISR(cts_queue, &intr_type, NULL);
    }
}
/* Ping callback */
IRAM_ATTR static bool ping_handle(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    int val = PING_REQUEST;
    xQueueSendFromISR(queue, &val, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR(NULL);
    }

    return xHigherPriorityTaskWoken;
}

static void start_ping_timer(mqtt_deamon_handler *handler)
{
    gptimer_config_t t_conf = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = PING_TIMER_RES // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&t_conf, &handler->ping_timer));
    /* Configure alarm time */
    gptimer_alarm_config_t t_alarm_conf = {
        .alarm_count = handler->keep_alive_int * PING_TIMER_RES,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true};
    ESP_ERROR_CHECK(gptimer_set_alarm_action(handler->ping_timer, &t_alarm_conf));
    /* Mount callback */
    gptimer_event_callbacks_t t_callb_conf = {
        .on_alarm = ping_handle};
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(handler->ping_timer, &t_callb_conf, handler->queue));
    ESP_ERROR_CHECK(gptimer_enable(handler->ping_timer));
    /* Start timer */
    ESP_ERROR_CHECK(gptimer_start(handler->ping_timer));
}

static void reload_ping_timer(mqtt_deamon_handler *handler)
{
    gptimer_set_raw_count(handler->ping_timer, 0);
}

static void stop_ping_timer(mqtt_deamon_handler *handler)
{
    /* Stop */
    gptimer_stop(handler->ping_timer);
    /* Disable callbacks */
    gptimer_disable(handler->ping_timer);
}

mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num)
{
    msg->ack = false;
    msg->num = num;

    return msg;
}

mqtt_msg *mqtt_msg_free(mqtt_msg *msg)
{
    msg->ack = true;

    return msg;
}

int mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        /* Create name */
        char *channel_name = get_channel_name(handler->username, INFO_CH_C);
        MQTTString channel_name_s = MQTTString_initializer;
        channel_name_s.cstring = channel_name;

        xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

        /* Set property */
        MQTTProperties ack_properties = MQTTProperties_initializer;

        /* Send ACK */
        int ser_len = MQTTV5Serialize_publish(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, QOS, 0, 0, channel_name_s, &ack_properties, buf, len);
        mqtt_tls_write(handler, handler->sendbuf, ser_len);

        xSemaphoreGive(handler->send_buf_mutex_handle);
        free(channel_name);
        return ser_len;
    }
    else
    {
        return -1;
    }
}

int mqtt_send_ack(mqtt_deamon_handler *handler, char master_num)
{
    /* Zero notack */
    handler->m_not_ack = 0;
    /* Send ack */
    char *data;
    size_t len = add_cmd(&data, 0, PACKET_ACK, &master_num);
    len = mqtt_write_i(handler, (unsigned char *)data, len);
    free(data);
    return len;
}

void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num)
{
    /* Decrement unack msgs */
    char temp = handler->slave_num - slave_num;

    if (temp < 0)
    {
        temp += MAX_MSG_NUM - MIN_MSG_NUM;
    }

    handler->s_not_ack -= temp;

    /* Delete msgs */
    char max = slave_num;
    char min;

    if ((slave_num % MIN_MSG_NUM) < (MAX_NOT_ACK))
    {
        min = MAX_MSG_NUM - (slave_num % MIN_MSG_NUM);
    }
    else
    {
        min = max - MAX_NOT_ACK;
    }

    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        if (max < min)
        {
            if (handler->send_msgs[i].num <= max || handler->send_msgs[i].num >= min)
            {
                handler->send_msgs[i].ack = true;
            }
        }
        else
        {
            if (handler->send_msgs[i].num <= max && handler->send_msgs[i].num >= min)
            {
                handler->send_msgs[i].ack = true;
            }
        }
    }
}

int mqtt_write_d(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (len > MAX_SLAVE_DATA_LEN)
    {
        /* Data too long */
        ESP_LOGE(TAG, "DATA TO SEND TOO LONG");
        return -2;
    }

    if (handler != NULL && handler->initialized == true)
    {
        /* Get name */
        char *channel_name = get_channel_name(handler->username, DATA_CH_C);
        MQTTString topic_str = MQTTString_initializer;
        topic_str.cstring = channel_name;

        xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);

        /* Find msg to delete */
        unsigned char *msg_data_ptr = NULL;
        for (unsigned char i = 0; i < MAX_SAVED; i++)
        {
            if (handler->send_msgs[i].ack == true)
            {
                /* Picking the first acked packet's place */
                mqtt_msg_intializer(&handler->send_msgs[i], handler->slave_num);
                msg_data_ptr = handler->send_msgs[i].data;
                break;
            }
        }

        /* No space found? */
        if (msg_data_ptr == NULL)
        {
            /* No space for data */
            ESP_LOGE(TAG, "NOT ENOUGH SPACE FOR NEW PACKET, CHOOSING THE OLDEST ONE");
            /* Find oldest packet */
            mqtt_msg *msg_ptr = &handler->send_msgs[0];
            for (unsigned char i = 0; i < MAX_SAVED; i++)
            {
                if ((handler->slave_num % MIN_MSG_NUM) < (MAX_NOT_ACK))
                {
                    if (msg_ptr->num > handler->slave_num && msg_ptr->num > handler->send_msgs[i].num)
                    {
                        msg_ptr = &handler->send_msgs[i];
                    }
                }
                else
                {
                    if (msg_ptr->num > handler->send_msgs[i].num)
                    {
                        msg_ptr = &handler->send_msgs[i];
                    }
                }
            }

            /* Set oldest packet as a place for a new one */
            mqtt_msg_intializer(msg_ptr, handler->slave_num);
            msg_data_ptr = msg_ptr->data;
        }

        /* Set property */
        MQTTProperty ack_properties_array[1];
        MQTTProperties ack_properties = MQTTProperties_initializer;
        ack_properties.array = ack_properties_array;
        ack_properties.max_count = 1;

        MQTTProperty packet_num = {
            .identifier = MQTTPROPERTY_CODE_USER_PROPERTY,
            .value.string_pair.key.data = PACKET_NUM_KEY,
            .value.string_pair.key.len = strlen(PACKET_NUM_KEY),
            .value.string_pair.val.data = (char *)&handler->slave_num,
            .value.string_pair.val.len = PACKET_NUM_MAX
        };
        MQTTProperties_add(&ack_properties, &packet_num);

        /* Send ACK */
        len = MQTTV5Serialize_publish(msg_data_ptr, MAX_SLAVE_PACKET_LEN, 0, QOS, 0, 0, topic_str, &ack_properties, buf, len);
        len = mqtt_tls_write(handler, msg_data_ptr, len);

        mqtt_num_up(&handler->slave_num);
        handler->s_not_ack++;
        ESP_LOGE(TAG, "NOT ACKED: %u", handler->s_not_ack);
        free(channel_name);
        xSemaphoreGive(handler->send_msgs_mutex_handle);

        if (handler->s_not_ack > MAX_NOT_ACK)
        {
            ESP_LOGE(TAG, "PACKET OVERFLOW MSG");
            char *msg = NULL;
            len = add_cmd_none(&msg, 0, PACKET_OVERFLOW);
            mqtt_write_i(handler, (unsigned char *)msg, len);
            free(msg);
        }

        return len;
    }
    else
    {
        return -1;
    }
}

int mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num)
{
    if (handler != NULL && handler->initialized == true)
    {
        int len;

        xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);

        mqtt_msg *msg = NULL;
        for (unsigned char i = 0; i < MAX_SAVED; i++)
        {
            if (handler->send_msgs[i].num == slave_num)
            {
                msg = &handler->send_msgs[i];
            }
        }

        if (msg == NULL)
        {
            xSemaphoreGive(handler->send_msgs_mutex_handle);
            return -2;
        }

        len = mqtt_tls_write(handler, msg->data, msg->len);

        xSemaphoreGive(handler->send_msgs_mutex_handle);

        return len;
    }
    else
    {
        return -1;
    }
}

static int mqtt_subscribe(mqtt_deamon_handler *handler, char channel)
{
    /* Subscribe to channels */
    char *channel_name = get_channel_name(handler->username, channel);
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = channel_name;
    MQTTProperties sub_properties = MQTTProperties_initializer;
    MQTTSubscribe_options sub_options = {.noLocal = 1, .retainAsPublished = 0, .retainHandling = 0};

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

    /* Send subscribe */
    int len = MQTTV5Serialize_subscribe(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, handler->msg_id++, &sub_properties, 1, &topicString, &handler->qos, &sub_options);
    len = mqtt_tls_write(handler, handler->sendbuf, len);

    xSemaphoreGive(handler->send_buf_mutex_handle);

    /* Deallocate name */
    free(channel_name);

    return len;
}

static int mqtt_tls_write_no_ping(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        xSemaphoreTake(handler->mbed_mutex_handle, portMAX_DELAY);
        ESP_LOGI(TAG, "TRYING SSL WRITE, socket: %i", handler->ctx.net_ctx.fd);

        size_t sent = 0;
        while (sent < len)
        {
            int rv = mbedtls_ssl_write(&handler->ctx.ssl_ctx, buf, len);
            if (rv < 0)
            {
                if (rv == MBEDTLS_ERR_SSL_WANT_READ ||
                    rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                    || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                    || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
                )
                {
                    /* should call mbedtls_ssl_write later again */
                    continue;
                }
                break;
            }
            /*
             * Note: rv can be 0 here eg. when mbedtls just flushed
             * the previous incomplete record.
             *
             * Note: we never send an empty TLS record.
             */
            sent += (size_t)rv;
        }
        if (sent == 0)
        {
            ESP_LOGI(TAG, "SSL WRITE, returned: 0");
            xSemaphoreGive(handler->mbed_mutex_handle);
            return -1;
        }
        /* Write to broker */

        ESP_LOGI(TAG, "SSL WRITE DONE, wrote: %i", len);
        xSemaphoreGive(handler->mbed_mutex_handle);
        /* Return */
        return sent;
    }
    else
    {
        if (handler == NULL)
            ESP_LOGI(TAG, "MQTT STRUCTURE NULL");
        else
            ESP_LOGI(TAG, "MQTT STRUCTURE NOT INITIALIZED");

        return -1;
    }
}

int mqtt_tls_write(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    int sent;

    if ((sent = mqtt_tls_write_no_ping(handler, buf, len)) > 0)
    {
        /* Reset timer */
        reload_ping_timer(handler);

        return sent;
    }
    else
    {
        return -1;
    }
}

static int mqtt_tls_read(mbedtls_ssl_context *ctx, unsigned char *buf, int len)
{
    int rv;

    do
    {
        rv = mbedtls_ssl_read(ctx, buf, len);
        if (rv > 0)
        {
            /* Enough data, return */
            return len;
        }
        else if (rv == 0)
        {
            /*
             * Note: mbedtls_ssl_read returns 0 when the underlying
             * transport was closed without CloseNotify.
             *
             * Raise an error to trigger a reconnect.
             */
            /* Socket closed */
            return -1;
        }
        else if (rv < 0)
        {
            if (rv == -1)
            {
                /* No data in buffer */
                rv = 0;
                return rv;
            }
            else if (rv == MBEDTLS_ERR_SSL_WANT_READ)
            {
                rv = 0;
                continue;
            }
            else if (rv == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
            {
                /* We were waiting for application data but got
                 * a NewSessionTicket instead. */
                /* Ignore it */
                rv = 0;
                continue;
            }
            else if (rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                     || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                     || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
            )
            {
                /* should call mbedtls_ssl_read later again */
                return 0;
            }
            /* Note: MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY is handled here. */
            // printf("NOT SERIOUS SOCKET ERROR?");
            rv = -1;
            break;
        }
    } while (1);

    return rv;
}

static void clean_properties(MQTTProperties *properties)
{
    properties->count = 0;
    properties->length = 0;
}

static void mqtt_deamon(void *v_mqtt_deamon_handler)
{
    int err = SIM_noErrCode;
    mqtt_deamon_handler *handler = (mqtt_deamon_handler *)v_mqtt_deamon_handler;
    uint8_t recvbuf[MAIN_MQTT_REC_BUF_SIZE];

    /* Used for properties reading */
    MQTTProperty properties_array[PROPERTIES_SIZE];
    MQTTProperties properties = MQTTProperties_initializer;
    properties.array = properties_array;
    properties.max_count = PROPERTIES_SIZE;
    /* QOS ptr */
    unsigned char qos = QOS;

    while (1)
    {
        err = SIM_noErrCode;
        /* Wait untill socket informs you that you got a new packet */
        if (xQueueReceive(handler->queue, &err, MQTT_UNLOCK_T / portTICK_PERIOD_MS) != pdTRUE)
        {
            /* Check if there is any data in buffer */
            if (handler->buf_check(&handler->ctx) > 0)
            {
                ESP_LOGI(TAG, "FOUND DATA");
                err = SIM_receive;
            }
            else
            {
                ESP_LOGI(TAG, "FALSE ALARM");
                continue;
            }
        }
        ESP_LOGI(TAG, "GOT MESSAGE, msg: %i", err);

        /* Check if sockt didn't get any error */
        if (err == SIM_closed || err == SIM_closedOk)
        {
            ESP_LOGE(TAG, "SOCKET CLOSED, RECONNECT REQUIRED");
            handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
            goto exit;
        }

        ESP_LOGI(TAG, "CHECK PING");
        /* Check if ping needed */
        if (err == PING_REQUEST)
        {
            ESP_LOGI(TAG, "SENDING PING");
            int len = MQTTV5Serialize_pingreq(recvbuf, MAIN_MQTT_REC_BUF_SIZE);
            if (mqtt_tls_write(handler, recvbuf, len) <= 0)
            {
                ESP_LOGE(TAG, "COULD NOT SEND DATA");
                handler->error_handler(&handler->handler, "MQTT", ext_type_non_fatal, -1);
                goto exit;
            }
            continue;
        }
        /* Check CTS */
        else if (err == CTS_CHANGED_STATE)
        {
            ESP_LOGI(TAG, "SENDING CTS UPDATE");
            char *msg;
            int len = 0;
            int state = gpio_get_level(MQTT_CTS_PIN);

            if (state == 1)
            {
                len = add_cmd_none(&msg, len, CTS_SET);
            }
            else
            {
                len = add_cmd_none(&msg, len, CTS_RESET);
            }

            if (mqtt_write_i(handler, (unsigned char *)msg, len) <= 0)
            {
                ESP_LOGE(TAG, "COULD NOT SEND DATA");
                handler->error_handler(&handler->handler, "MQTT", ext_type_non_fatal, -1);
                goto exit;
            }
            continue;
        }

        if ((err = MQTTV5Packet_readnb(recvbuf, MAIN_MQTT_REC_BUF_SIZE, &handler->trp)) >= 0)
        {
            switch (err)
            {
            case 0: /* timed out reading packet */
            {
                break;
            }
            case CONNACK:
            {
                ESP_LOGI(TAG, "CONNACK RECEIVED");

                /* Check if connected */
                unsigned char sessionPresent, connack_rc;

                if (MQTTV5Deserialize_connack(&properties, &sessionPresent, &connack_rc, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1 || connack_rc != 0)
                {
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, connack_rc);
                    goto exit;
                }

                /* Find keep alive time */
                ESP_LOGI(TAG, "PROPERTIES, size: %i", properties.count);
                for (unsigned int i = 0; i < properties.count; i++)
                {
                    ESP_LOGI(TAG, "PROPERTY, type: %i", properties.array[i].identifier);

                    if (properties.array[i].identifier == MQTTPROPERTY_CODE_SERVER_KEEP_ALIVE)
                    {
                        handler->keep_alive_int = properties.array[i].value.integer2 - 30;
                        ESP_LOGI(TAG, "KEEP ALIVE, rec: %i", properties.array[i].value.integer2);
                        ESP_LOGI(TAG, "KEEP ALIVE: %i", handler->keep_alive_int);
                    }
                    else
                    {
                        handler->keep_alive_int = 60;
                        ESP_LOGI(TAG, "KEEP ALIVE: %i", handler->keep_alive_int);
                    }
                }

                /* Start Ping timer */
                start_ping_timer(handler);

                /* Subscribe to channels */
                ESP_LOGE(TAG, "SENDING SUBSCRIBE TO CHANNEL: %c", INFO_CH_C);
                mqtt_subscribe(handler, INFO_CH_C);
                mqtt_subscribe(handler, SET_CH_C);
                mqtt_subscribe(handler, DATA_CH_C);

                break;
            }
            case SUBACK:
            {
                /* Check if last subscribe ok */
                // unsigned short submsg_id;
                unsigned short msg_id;
                int subcount;
                unsigned char reason_code;

                if (MQTTV5Deserialize_suback(&msg_id, &properties, 1, &subcount, &reason_code, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1 || reason_code != 0)
                {
                    ESP_LOGE(TAG, "SUBSCRIBE ERROR");
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, reason_code);
                    goto exit;
                }

                break;
            }
            case PUBLISH:
            {
                /* NOT USED */
                unsigned char dup;
                unsigned char qos_;
                unsigned char retained;
                unsigned short packet_id;
                /* USED */
                MQTTString topic_name;
                char *payload;
                int32_t payload_len;

                if (MQTTV5Deserialize_publish(&dup, &qos_, &retained, &packet_id, &topic_name, &properties, (unsigned char **)&payload, &payload_len, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1)
                {
                    ESP_LOGE(TAG, "NOT CORRECTLY FORMATTED DATA PACKET");
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, -1);
                    goto exit;
                }

                if (topic_name.lenstring.len < USERNAME_LEN +  1)
                    break;
                
                switch (topic_name.lenstring.data[USERNAME_LEN])
                {
                /* Info arrived */
                case INFO_CH_C:
                {
                    handle_info_channel(handler, (unsigned char *)payload, payload_len);

                    break;
                }
                /* Settings arrived */
                case SET_CH_C:
                {
                    handle_set_channel(handler, (unsigned char *)payload, payload_len);

                    break;
                }
                /* Data arrived */
                case DATA_CH_C:
                {
                    /* Allow to write only if flow_ctrl is disabled
                     * or Clear To Send was enabled.
                     * Otherwise throw away this packet.
                     */
                    /* Find packet number */
                    char *data_to_send = NULL;
                    bool num_found = false;
                    char packet_num;
                    for (unsigned char i = 0; i < properties.count; i++)
                    {
                        /* Property found */
                        if (properties.array[i].identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
                        {
                            /* Get packet number */
                            num_found = true;
                            packet_num = properties.array[i].value.string_pair.val.data[0];

                            if (handler->m_clean_session == true)
                            {
                                /* Master initialized clean session, set packet number */
                                handler->m_clean_session = false;
                                handler->master_num = packet_num;
                            }
                        }
                    }

                    if (num_found == false)
                    {
                        /* Send no packet number error, throw away this packet */
                        ESP_LOGE(TAG, "NO_PACKET_NUMBER");
                        size_t len = add_cmd_none(&data_to_send, 0, NO_PACKET_NUMBER);
                        mqtt_write_i(handler, (unsigned char *)data_to_send, len);
                        free(data_to_send);
                    }
                    else if (packet_num != handler->master_num)
                    {
                        /* Send invalid packet number error, ask for valid one, throw away this packet */
                        ESP_LOGE(TAG, "INVALID_PACKET_NUM");
                        size_t len = add_cmd(&data_to_send, 0, INVALID_PACKET_NUM, &handler->master_num);
                        mqtt_write_i(handler, (unsigned char *)data_to_send, len);
                        free(data_to_send);
                    }
                    else
                    {
                        /* Increment unack */
                        handler->m_not_ack++;

                        /* Send data to UART */
                        handler->publish_callb((unsigned char *)payload, payload_len);
                        /* Send ack if exceded ACK_AFTER */
                        if (handler->m_not_ack > ACK_AFTER)
                        {
                            mqtt_send_ack(handler, handler->master_num);
                            handler->m_not_ack = 0;
                        }
                        /* Set next packet number */
                        mqtt_num_up(&handler->master_num);
                    }
                    break;
                }
                }

                break;
            }
            case DISCONNECT:
            {
                /* This should never happen */
                ESP_LOGE(TAG, "GOT DISCONNECTED");
                handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, -1);
                goto exit;
                break;
            }
            default:
                break;
            }
        }
        else
        {
            /* ERROR */
            switch (err)
            {
            default:
            {
                handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, err);
                goto exit;
            }
            }
        }
    }

/* Send error to main deamon */
// TaskHandle_t task = xTaskGetCurrentTaskHandle();
// xQueueSend(handler->error_queue, &task, portMAX_DELAY);
// handler->hard_error_handler(&handler->handler);

/* wait for the task to be deleted */
exit:
    stop_ping_timer(handler);
    for (;;)
        ;
}

void mqtt_deamon_awake(mqtt_deamon_handler *handler, int *err)
{
    xQueueSend(handler->queue, err, portMAX_DELAY);
}

// TaskHandle_t *mqtt_deamon_get_task()
// {
//     static TaskHandle_t handle = NULL;
//     return &handle;
// }

static TaskHandle_t mqtt_deamon_create_task(mqtt_deamon_handler *handler)
{
    if (!(handler->handler))
    {
        xTaskCreate(mqtt_deamon, "mqtt_deamon", 20000, handler, 3, &handler->handler);
    }
    return handler->handler;
}

static TaskHandle_t mqtt_deamon_delete_task(mqtt_deamon_handler *handler)
{
    if (handler->handler)
    {
        vTaskDelete(handler->handler);
        handler->handler = NULL;
    }
    return handler->handler;
}

void set_mode(dev_mode mode)
{
    /* Reset gpio */
    gpio_reset_pin(MQTT_CTS_PIN);
    gpio_reset_pin(MQTT_RTS_PIN);
    
    /* GPIO RTS, CTS */
    const gpio_config_t cts_config = {
        .pin_bit_mask = BIT(MQTT_CTS_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE};
    gpio_config(&cts_config);

    const gpio_config_t rts_config = {
        .pin_bit_mask = BIT(MQTT_RTS_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE};
    gpio_config(&rts_config);

    gpio_set_intr_type(MQTT_RTS_PIN, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    
    if (mode == dte)
    {
        gpio_isr_handler_add(MQTT_RTS_PIN, cts_handle, (void *)CTS_CHANGED_STATE);
    }
    else
    {
        gpio_isr_handler_add(MQTT_RTS_PIN, cts_handle, (void *)RTS_CHANGED_STATE);
    }
}

int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      uart_config_t *uart_conf,
                      char *server,
                      char *port,
                      char *username,
                      char *password,
                      char **chain,
                      void (*socket_resp_handler)(int *err))
{
    int err;
    /* Prepare mutexes */
    handler->mbed_mutex_handle = xSemaphoreCreateBinary();
    xSemaphoreGive(handler->mbed_mutex_handle);
    handler->send_buf_mutex_handle = xSemaphoreCreateBinary();
    xSemaphoreGive(handler->send_buf_mutex_handle);
    handler->send_msgs_mutex_handle = xSemaphoreCreateBinary();
    xSemaphoreGive(handler->send_msgs_mutex_handle);
    /* Queues */
    handler->queue = xQueueCreate(10, sizeof(int));
    cts_queue = handler->queue;
    /* Set username */
    handler->username = username;
    /* Set valid msg numbers */
    handler->msg_id = 1;
    handler->m_not_ack = 0;
    handler->m_not_ack = 0;
    handler->m_clean_session = true;
    handler->slave_num = MIN_MSG_NUM;
    handler->uart_conf = uart_conf;
    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        handler->send_msgs[i].ack = true;
    }

    /* GPIO RTS, CTS */
    set_mode(dce);

    /* Init MQTT client */
    handler->trp.sck = (void *)&handler->ctx.ssl_ctx; // set contrxt
    handler->trp.getfn = mqtt_tls_read;               // set read function
    handler->trp.state = 0;

    /* Open encrypted socket */
    if ((err = open_nb_socket(&handler->ctx, server, port, (unsigned char **)chain)) != 0)
    {
        return err;
    }
    // set handler that awakes deamon when new message comes
    if ((err = socket_set_handler(&handler->ctx, socket_resp_handler)) != 0)
    {
        return err;
    }

    /* SSL initialized */
    handler->initialized = true;

    /* Will channel */
    char *channel_name = get_channel_name(handler->username, INFO_CH_C);
    MQTTString will_topic = MQTTString_initializer;
    will_topic.cstring = channel_name;

    /* Will message */
    char *will_str = NULL;
    add_cmd_none(&will_str, 0, DEV_DISCONNECT);
    MQTTString will_msg = MQTTString_initializer;
    will_msg.cstring = will_str;

    /* Will */
    MQTTV5Packet_willOptions will = MQTTV5Packet_willOptions_initializer;
    will.topicName = will_topic;
    will.message = will_msg;
    will.qos = QOS;

    /* Connection parameters */
    MQTTV5Packet_connectData connect_data = MQTTV5Packet_connectData_initializer;
    connect_data.username.cstring = username;
    connect_data.password.cstring = password;
    connect_data.MQTTVersion = 5;
    connect_data.keepAliveInterval = 90;
    // connect_data.willFlag = 1;
    // connect_data.will = will;

    /* Connection properties, none needed */
    MQTTProperties conn_properties = MQTTProperties_initializer;

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);
    int len = MQTTV5Serialize_connect(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, &connect_data, &conn_properties);
    int write_len = mqtt_tls_write_no_ping(handler, handler->sendbuf, len);
    xSemaphoreGive(handler->send_buf_mutex_handle);

    free(channel_name);
    free(will_str);

    if (write_len == len)
    {
        /* Start deamon */
        mqtt_deamon_create_task(handler);
        if (!handler->handler)
        {
            /* Couldn't start deamon */
            return -1;
        }

        return 0;
    }
    else
    {
        return -1;
    }
}

int mqtt_deamon_stop(mqtt_deamon_handler *handler)
{
    vSemaphoreDelete(handler->mbed_mutex_handle);
    vSemaphoreDelete(handler->send_buf_mutex_handle);
    vSemaphoreDelete(handler->send_msgs_mutex_handle);
    mqtt_deamon_delete_task(handler);
    vQueueDelete(handler->queue);

    while (handler->handler)
        ;

    return 0;
    // mbedtls_net_context ctx = {.fd = handler->socket_num};

    // if (!(handler->handler))
    // {
    //     // TODO close socket, clean up
    //     close_nb_socket(&ctx);
    //     if (ctx.fd == -1)
    //         return 0;
    //     else
    //         return -2;
    // }
    // else
    // {
    //     return -1;
    // }
}