#include <mqtt_deamon.h>

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_system.h>
/* SIM800-specyfic libraries */
#include <SIM.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
/* Main deamon */
#include <error_handler.h>
#include <cert.h>
/* UART deamon */
#include <uart_deamon.h>
/*  */
#include <cmd_defs.h>
/* MQTT topics handling */
#include <info_ch.h>
#include <set_ch.h>
/* RTS CTS pin */
#include <rts_cts.h>
/* Queue defs */
#include <mqtt_queue.h>
/* MQTT pinging */
#include <mqtt_ping.h>

extern mqtt_deamon_handler mqtt_handler;

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

                if (topic_name.lenstring.len < USERNAME_LEN + 1)
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

/* wait for the task to be deleted */
exit:
    stop_ping_timer(handler);
    for (;;)
        ;
}

static void mqtt_deamon_awake(mqtt_deamon_handler *handler, int *err)
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

static void socket_resp_handler(int *err) //TODO pass handler as an argument
{
    mqtt_deamon_awake(&mqtt_handler, err); 
}

int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      uart_config_t *uart_conf,
                      char *server,
                      char *port,
                      char *username,
                      char *password,
                      unsigned char (*get_cert)(unsigned char),
                      unsigned char chain_size)
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
        handler->send_msgs[i].num = 0;
    }

    /* GPIO RTS, CTS */
    rts_cts_set_mode(dce, handler->queue);

    /* Init MQTT client */
    handler->trp.sck = (void *)&handler->ctx.ssl_ctx; // set contrxt
    handler->trp.getfn = mqtt_tls_read;               // set read function
    handler->trp.state = 0;

    /* Open encrypted socket */
    if ((err = open_nb_socket(&handler->ctx, server, port, get_cert, chain_size)) != 0)
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
    size_t len_ = add_cmd_none(&will_str, 0, DEV_DISCONNECT);
    will_str = realloc(will_str, sizeof(char) * len_ + 1);
    will_str[len_] = '\0';
    MQTTString will_msg = MQTTString_initializer;
    will_msg.cstring = will_str;

    /* Will properties */
    MQTTProperties will_props = MQTTProperties_initializer;

    /* Will */
    MQTTV5Packet_willOptions will = MQTTV5Packet_willOptions_initializer;
    will.topicName = will_topic;
    will.message = will_msg;
    will.qos = QOS;
    will.properties = &will_props;

    /* Connection parameters */
    MQTTV5Packet_connectData connect_data = MQTTV5Packet_connectData_initializer;
    connect_data.username.cstring = username;
    connect_data.password.cstring = password;
    connect_data.MQTTVersion = 5;
    connect_data.keepAliveInterval = 90;
    connect_data.willFlag = 1;
    connect_data.will = will;

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