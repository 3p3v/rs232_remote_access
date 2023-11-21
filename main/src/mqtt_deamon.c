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
static mqtt_deamon_handler *mdh = NULL;

// static SemaphoreHandle_t mutex;
// static StaticSemaphore_t xMutexBuffer;

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
        .alarm_count = ((float)handler->keep_alive_int / (float)SEC_IN_MIN) * (float)PING_TIMER_RES,
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

int mqtt_write_d(unsigned char *buf, size_t len)
{
    if (mdh != NULL && mdh->initialized == true)
    {
        static char channel_name[19] = {0};
        static bool initialized = false;

        if (initialized == false)
        {
            strcpy(channel_name, mdh->username);
            channel_name[sizeof(channel_name) - 1] = INFO_CH_C;
        }

        xSemaphoreTake(mdh->send_buf_mutex_handle, portMAX_DELAY);

        /* Create name */
        MQTTString topicString = MQTTString_initializer;
        topicString.cstring = channel_name;

        /* Set property */
        MQTTProperty ack_properties_array[1];
        MQTTProperties ack_properties = MQTTProperties_initializer;
        ack_properties.count = 1;
        ack_properties.array = ack_properties_array;
        ack_properties_array[0].identifier = MQTTPROPERTY_CODE_USER_PROPERTY;
        char slave_num_str[12];
        sprintf(slave_num_str, "%u", mdh->slave_num++);
        char key[] = "N";
        ack_properties_array[0].value.string_pair.key.data = key;
        ack_properties_array[0].value.string_pair.key.len = strlen(key);
        ack_properties_array[0].value.string_pair.val.data = slave_num_str;
        ack_properties_array[0].value.string_pair.val.len = strlen(slave_num_str);

        /* Send ACK */
        len = MQTTV5Serialize_publish(mdh->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, QOS, 0, 0, topicString, &ack_properties, buf, len);
        len = mqtt_tls_write(mdh->sendbuf, len);

        xSemaphoreGive(mdh->send_buf_mutex_handle);

        return len;
    }

    return -1;
}

int mqtt_tls_write(unsigned char *buf, size_t len)
{
    if (mdh != NULL && mdh->initialized == true)
    {
        /* Write to broker */
        xSemaphoreTake(mdh->mbed_mutex_handle, portMAX_DELAY);
        ESP_LOGI(TAG, "TRYING SSL WRITE, socket: %i", mdh->ctx.net_ctx.fd);
        len = mbedtls_ssl_write(&mdh->ctx.ssl_ctx, buf, len);
        ESP_LOGI(TAG, "SSL WRITE DONE, wrote: %i", len);
        xSemaphoreGive(mdh->mbed_mutex_handle);
        /* Reset timer */
        reload_ping_timer(mdh);
        /* Return */
        return len;
    }
    else
    {
        return -1;
    }
}

static int mqtt_tls_write_no_ping(unsigned char *buf, size_t len)
{
    if (mdh != NULL && mdh->initialized == true)
    {
        xSemaphoreTake(mdh->mbed_mutex_handle, portMAX_DELAY);
        ESP_LOGI(TAG, "TRYING SSL WRITE, socket: %i", mdh->ctx.net_ctx.fd);
        
        size_t sent = 0;
        while (sent < len)
        {
            int rv = mbedtls_ssl_write(&mdh->ctx.ssl_ctx, buf, len);
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
            return -1;
        }
        return (ssize_t)sent;
        /* Write to broker */
        
        ESP_LOGI(TAG, "SSL WRITE DONE, wrote: %i", len);
        xSemaphoreGive(mdh->mbed_mutex_handle);
        /* Return */
        return sent;
    }
    else
    {
        return -1;
    }
}

// static int mqtt_packet_ack(mqtt_deamon_handler *handler, unsigned char *buf, size_t len, MQTTProperty *packet_num_prop)
// {
//     /* Create name */
//     MQTTString topicString = MQTTString_initializer;
//     char *channel_name = (char *)malloc(sizeof(char) * (strlen(handler->username) + 2));
//     memcpy(channel_name, handler->username, strlen(handler->username));
//     channel_name[strlen(channel_name)] = 'i';
//     topicString.cstring = channel_name;

//     /* Set property */
//     MQTTProperty ack_properties_array[1];
//     MQTTProperties ack_properties = MQTTProperties_initializer;
//     ack_properties.count = 1;
//     ack_properties.array = ack_properties_array;
//     ack_properties_array[0] = *packet_num_prop;

//     /* Send ACK */
//     int len = MQTTV5Serialize_publish(buf, len, 0, QOS, 0, 0, topicString, &ack_properties, NULL, 0);
//     mqtt_tls_write(recbuf, len);

//     free(channel_name);
// }

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

    /* Id of message to send */
    unsigned short msg_id = 200;
    /* Used for choosing channel to connect */
    unsigned char ch_num = 0;
    // if ever received publish from master

    /* Used for properties reading */
    MQTTProperty properties_array[PROPERTIES_SIZE];
    MQTTProperties properties = MQTTProperties_initializer;
    properties.array = properties_array;
    properties.max_count = PROPERTIES_SIZE;
    /* QOS ptr */
    unsigned char qos = QOS;

    while (1)
    {
        /* Wait untill socket informs you that you got a new packet */
        xQueueReceive(handler->queue, &err, portMAX_DELAY);
        ESP_LOGI(TAG, "GOT MESSAGE");

        /* Check if sockt didn't get any error */
        if (err == SIM_closed || err == SIM_closedOk)
        {
            handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
            break;
        }

        ESP_LOGI(TAG, "CHECK PING");
        /* Check if ping needed */
        if (err == PING_REQUEST)
        {
            int len = MQTTV5Serialize_pingreq(recvbuf, MAIN_MQTT_REC_BUF_SIZE);
            mqtt_tls_write(recvbuf, len);
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
                        ESP_LOGI(TAG, "KEEP ALIVE: %i", properties.array[i].value.integer2);
                    }
                    else
                    {
                        handler->keep_alive_int = 30;
                    }
                }

                /* Start Ping timer */
                start_ping_timer(handler);

                /* Subscribe to channels */
                MQTTString topicString = MQTTString_initializer;
                MQTTProperties sub_properties = MQTTProperties_initializer;
                MQTTSubscribe_options sub_options = {.noLocal = 1, .retainAsPublished = 0, .retainHandling = 0};

                /* Create name */
                char *channel_name = (char *)malloc(sizeof(char) * (strlen(handler->username) + 2));
                memcpy(channel_name, handler->username, strlen(handler->username));
                channel_name[strlen(channel_name)] = 'i';

                /* Assign name */
                topicString.cstring = channel_name;

                /* Send subscribe */
                int len = MQTTV5Serialize_subscribe(recvbuf, MAIN_MQTT_REC_BUF_SIZE, 0, msg_id++, &sub_properties, 1, &topicString, &qos, &sub_options);
                mqtt_tls_write(recvbuf, len);

                /* Deallocate name */
                free(channel_name);

                reload_ping_timer(handler);

                break;
            }
            case SUBACK:
            {
                /* Check if last subscribe ok */
                // unsigned short submsg_id;
                int subcount;
                unsigned char reason_code;

                if (MQTTV5Deserialize_suback(&msg_id, &properties, 1, &subcount, &reason_code, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1 || reason_code != 0)
                {
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, reason_code);
                    goto exit;
                }

                /* Subscribe to channels */
                MQTTString topicString = MQTTString_initializer;
                MQTTProperties sub_properties = MQTTProperties_initializer;
                MQTTSubscribe_options sub_options = {.noLocal = 1, .retainAsPublished = 0, .retainHandling = 0};
                char *channel_name = (char *)malloc(sizeof(char) * (strlen(handler->username) + 2));
                memcpy(channel_name, handler->username, strlen(handler->username));
                topicString.cstring = channel_name;

                if (ch_num == 0)
                {
                    ch_num++;
                    channel_name[strlen(channel_name)] = 's';
                }
                else
                {
                    channel_name[strlen(channel_name)] = 'd';
                }

                /* Send subscribe */
                int len = MQTTV5Serialize_subscribe(recvbuf, MAIN_MQTT_REC_BUF_SIZE, 0, msg_id++, &sub_properties, 1, &topicString, &qos, &sub_options);
                mqtt_tls_write(recvbuf, len);

                /* Deallocate name */
                free(channel_name);

                reload_ping_timer(handler);

                break;
            }
            case PUBLISH:
            {
                unsigned char dup;
                unsigned char qos_;
                unsigned char retained;
                unsigned short packet_id;
                MQTTString topic_name;
                char *payload;
                int32_t payload_len;

                if (MQTTV5Deserialize_publish(&dup, &qos_, &retained, &packet_id, &topic_name, &properties, (unsigned char **)&payload, &payload_len, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1)
                {
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, -1);
                    goto exit;
                }

                switch (topic_name.cstring[USERNAME_LEN])
                {
                case 'i':
                {
                    handle_info_channel(handler, (unsigned char *)payload, payload_len);

                    break;
                }
                case 's':
                {
                    handle_set_channel(handler, (unsigned char *)payload, payload_len);

                    break;
                }
                case 'd':
                {
                    /* Find packet number */
                    bool num_found = false;
                    char data_num_str[10] = {0};
                    unsigned int data_num_prop;
                    for (unsigned char i = 0; i < properties.count; i++)
                    {
                        if (properties.array[i].identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
                        {
                            memcpy(data_num_str, properties.array[i].value.string_pair.val.data, properties.array[i].value.string_pair.val.len);
                            data_num_prop = atoi(data_num_str);
                            num_found = true;
                        }
                    }

                    if (num_found == false)
                    {
                        handler->error_handler(&handler->handler, "MQTT", ext_type_non_fatal, -1);
                    }
                    else if (data_num_prop != handler->master_num++)
                    {
                        handler->error_handler(&handler->handler, "MQTT", ext_type_non_fatal, -1);
                        // if (properties.array[data_num_prop].value.integer4 != )
                        // /* Send ACK */
                        // mqtt_packet_ack(handler, sendbuf, MAIN_MQTT_SEND_BUF_SIZE, properties.array + data_num_prop);
                    }

                    /* Send data to UART */
                    handler->publish_callb((unsigned char *)payload, payload_len);

                    break;
                }
                }
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
        // printf("MQTT error: %i", err);
        err = SIM_noErrCode;
    }

/* Send error to main deamon */
// TaskHandle_t task = xTaskGetCurrentTaskHandle();
// xQueueSend(handler->error_queue, &task, portMAX_DELAY);
// handler->hard_error_handler(&handler->handler);

/* wait for the task to be deleted */
exit:
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
    stop_ping_timer(handler);

    if (handler->handler)
    {
        vTaskDelete(handler->handler);
        handler->handler = NULL;
    }
    return handler->handler;
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
    /* Assign handler data to ptr */
    mdh = handler;
    /* Set username */
    handler->username = username;
    /* Set valid msg numbers */
    handler->uart_conf = uart_conf;

    /* Init MQTT client */
    handler->trp.sck = (void *)&handler->ctx.ssl_ctx; // set contrxt
    handler->trp.getfn = mqtt_tls_read;               // set read function
    handler->trp.state = 0;

    /* TLS structures */
    handler->queue = xQueueCreate(10, sizeof(int));

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
    /* Save ctx */

    /* SSL initialized */
    mdh->initialized = true;

    /* Connection parameters */
    MQTTV5Packet_connectData connect_data = MQTTV5Packet_connectData_initializer;
    connect_data.username.cstring = username;
    connect_data.password.cstring = password;
    connect_data.MQTTVersion = 5;
    connect_data.keepAliveInterval = 60;
    /* Connection properties, none needed */
    MQTTProperties conn_properties = MQTTProperties_initializer;

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);
    ESP_LOGI(TAG, "CONNECT SERIALIZE");
    int len = MQTTV5Serialize_connect(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, &connect_data, &conn_properties);
    ESP_LOGI(TAG, "CONNECT SERIALIZED");
    int write_len = mqtt_tls_write_no_ping(handler->sendbuf, len);
    ESP_LOGI(TAG, "CONNECT SENT?");
    xSemaphoreGive(handler->send_buf_mutex_handle);

    /* Start deamon */
    mqtt_deamon_create_task(handler);
    if (!handler->handler)
    {
        /* Couldn't start deamon */
        return -1;
    }

    /* Connect to broker */
    if (write_len == len)
    {
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