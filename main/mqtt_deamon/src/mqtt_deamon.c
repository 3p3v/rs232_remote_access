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
#include <auth_defs.h>
/* MQTT topics handling */
#include <info_ch.h>
#include <set_ch.h>
/* RTS CTS pin */
#include <rts_cts.h>
/* Queue defs */
#include <mqtt_queue.h>
/* MQTT pinging */
#include <mqtt_ping.h>
#include <mqtt_ch_rw.h>
#include <mqtt_rw.h>

extern mqtt_deamon_handler mqtt_handler;

static void handle_disconnect(mqtt_deamon_handler *handler)
{
    /* This should never happen */
    ESP_LOGE(TAG, "GOT DISCONNECTED");
    handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, mqtt_deamon_disconnected);
}

static mqtt_daemon_code handle_connack(mqtt_deamon_handler *handler, unsigned int *keep_alive)
{
    if (keep_alive)
    {
        handler->keep_alive_int = keep_alive - MQTT_DEAMON_DEF_PING_OFFSET;
    }
    else
    {
        handler->keep_alive_int = MQTT_DEAMON_DEF_PING_TIME;
    }

    ESP_LOGI(TAG, "KEEP ALIVE: %i", handler->keep_alive_int);
    
    /* Start Ping timer */
    start_ping_timer(handler);

    /* Subscribe to channels */
    int err;
    if ((err = mqtt_subscribe(handler, INFO_CH_C)) < 0)
    {
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, err);
        
        return err;
    }
    else if ((err = mqtt_subscribe(handler, SET_CH_C)) < 0)
    {
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, err);
        
        return err;
    }
    else if ((err = mqtt_subscribe(handler, DATA_CH_C)) < 0)
    {
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, err);
        
        return err;
    }

    return mqtt_daemon_ok;
}

static mqtt_daemon_code handle_suback(mqtt_deamon_handler *handler, int reason_code)
{
    if (err)
    {
        /* This should never happen */
        ESP_LOGE(TAG, "SUBSCRIBE ERROR");
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, mqtt_daemon_subscribe_err);
        
        return mqtt_daemon_subscribe_err;
    }

    return mqtt_daemon_ok;
}

static void handle_error(mqtt_deamon_handler *handler, mqtt_daemon_code err)
{
    handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, err);
}

static mqtt_daemon_code handle_rts_cts(mqtt_deamon_handler *handler)
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

    if (mqtt_write_i(handler, (unsigned char *)msg, len) < 0)
    {
        ESP_LOGE(TAG, "COULD NOT SEND DATA");
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, mqtt_daemon_werr);

        return mqtt_daemon_werr;
    }

    return mqtt_daemon_ok;
}

static mqtt_daemon_code handle_ping(mqtt_deamon_handler *handler)
{
    ESP_LOGI(TAG, "SENDING PING");
    int len = MQTTV5Serialize_pingreq(recvbuf, MAIN_MQTT_REC_BUF_SIZE);

    if (mqtt_tls_write(handler, recvbuf, len) < 0)
    {
        ESP_LOGE(TAG, "COULD NOT SEND DATA");
        handler->error_handler(&handler->handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, mqtt_daemon_werr);

        return mqtt_daemon_werr;
    }

    return mqtt_daemon_ok;
}

static void handle_socket_closed(mqtt_deamon_handler *handler)
{
    ESP_LOGE(TAG, "SOCKET CLOSED, RECONNECT REQUIRED");
    handler->error_handler(&handler->handler, "SIM", ext_type_fatal, mqtt_deamon_disconnected);
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
            handle_socket_closed(handler);

            goto EXIT;
        }

        ESP_LOGI(TAG, "CHECK PING");
        /* Check if ping needed */
        if (err == PING_REQUEST)
        {
            if (handle_ping(handler))
            {
                goto EXIT;
            }

            continue;
        }
        /* Check CTS */
        else if (err == CTS_CHANGED_STATE)
        {
            if (handle_rts_cts(handler))
            {
                goto EXIT;
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

                if (MQTTV5Deserialize_connack(&properties, &sessionPresent, &connack_rc, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1)
                {
                    handle_error(handler, mqtt_daemon_deserialize_error);
                    goto EXIT;
                }
                else if (connack_rc)
                {
                    handle_error(handler, mqtt_daemon_connect_err);
                    goto EXIT;
                }

                /* Find keep alive time */
                unsigned int ka;
                bool if_found_ka = false;

                ESP_LOGI(TAG, "PROPERTIES, size: %i", properties.count);
                for (unsigned int i = 0; i < properties.count; i++)
                {
                    ESP_LOGI(TAG, "PROPERTY, type: %i", properties.array[i].identifier);

                    if (properties.array[i].identifier == MQTTPROPERTY_CODE_SERVER_KEEP_ALIVE)
                    {
                        ka = properties.array[i].value.integer2 - 30;
                        if_found_ka = true;

                        ESP_LOGI(TAG, "KEEP ALIVE, rec: %i", properties.array[i].value.integer2);
                    }
                }

                if (if_found_ka)
                {
                    handle_connack(handler, &ka);
                }
                else
                {
                    handle_connack(handler, NULL);
                }

                break;
            }
            case SUBACK:
            {
                /* Check if last subscribe ok */
                // unsigned short submsg_id;
                unsigned short msg_id;
                int subcount;
                unsigned char reason_code;

                if (MQTTV5Deserialize_suback(&msg_id, &properties, 1, &subcount, &reason_code, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1)
                {
                    handle_error(handler, mqtt_daemon_deserialize_error);
                    goto EXIT;
                }
                else if (reason_code != 0)
                {
                    handle_error(handler, mqtt_daemon_subscribe_err);
                    goto EXIT;
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
                    handle_error(handler, mqtt_daemon_deserialize_error);
                    goto EXIT;
                }

                if (topic_name.lenstring.len < USERNAME_LEN + 1)
                    break;

                switch (topic_name.lenstring.data[USERNAME_LEN])
                {
                /* Info arrived */
                case INFO_CH_C:
                {
                    if (handle_info_channel(handler, (unsigned char *)payload, payload_len))
                    {
                        goto EXIT;
                    }

                    break;
                }
                /* Settings arrived */
                case SET_CH_C:
                {
                    if (handle_set_channel(handler, (unsigned char *)payload, payload_len))
                    {
                        goto EXIT;
                    }

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

                    if (handle_data_channel(handler, (unsigned char *)payload, payload_len, packet_num, num_found))
                    {
                        goto EXIT;
                    }

                    break;
                }
                }

                break;
            }
            case DISCONNECT:
            {
                handle_disconnect(handler);

                goto EXIT;
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
                handle_error(&handler->handler, mqtt_daemon_uerr);
                goto EXIT;
            }
            }
        }
    }

/* wait for the task to be deleted */
EXIT:
    stop_ping_timer(handler);
    for (;;)
        ;
}

static void mqtt_deamon_awake(mqtt_deamon_handler *handler, int *err)
{
    xQueueSend(handler->queue, err, portMAX_DELAY);
}

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

static void socket_resp_handler(int *err) // TODO pass handler as an argument
{
    mqtt_deamon_awake(&mqtt_handler, err);
}

static void mqtt_daemon_struct_init(mqtt_deamon_handler *handler, const char *username)
{
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
    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        handler->send_msgs[i].ack = true;
        handler->send_msgs[i].num = 0;
    }

    /* Init MQTT client */
    handler->trp.sck = (void *)&handler->ctx.ssl_ctx; // set contrxt
    handler->trp.getfn = mqtt_tls_read;               // set read function
    handler->trp.state = 0;
}

mqtt_daemon_code mqtt_deamon_start(mqtt_deamon_handler *handler,
                      char *server,
                      char *port,
                      char *username,
                      char *password,
                      unsigned char (*get_cert)(unsigned char),
                      unsigned char chain_size)
{
    mqtt_daemon_code err;

    mqtt_daemon_struct_init(handler, username);

    
    if (/* Open encrypted socket */
        socket_open_nb(&handler->ctx, server, port, get_cert, chain_size) ||
        /* set handler that awakes deamon when new message comes */s
        socket_set_handler(&handler->ctx, socket_resp_handler))
    {
        return mqtt_daemon_socket_err;
    }

    /* SSL initialized */
    handler->initialized = true;

    /* GPIO RTS, CTS */
    rts_cts_set_mode(dce, handler->queue);

    /* Connect to broker */
    err = mqtt_connect(handler, username, password);

    if (err == mqtt_daemon_ok)
    {
        /* Start deamon */
        mqtt_deamon_create_task(handler);
        if (!handler->handler)
        {
            /* Couldn't start deamon */
            return mqtt_daemon_uerr;
        }
    }

    return err;

}

mqtt_daemon_code mqtt_deamon_stop(mqtt_deamon_handler *handler)
{
    vSemaphoreDelete(handler->mbed_mutex_handle);
    vSemaphoreDelete(handler->send_buf_mutex_handle);
    vSemaphoreDelete(handler->send_msgs_mutex_handle);
    mqtt_deamon_delete_task(handler);
    vQueueDelete(handler->queue);

    while (handler->handler)
        ;

    close_nb_socket(&handler->ctx);
    if (handler->ctx.net_ctx.fd == -1)
        return mqtt_daemon_ok;
    else
        return mqtt_daemon_socket_err;
}