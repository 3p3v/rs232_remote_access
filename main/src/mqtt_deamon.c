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
#include <mbedtls_sockets.h>
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
/* MQTT-C lib */
#include <MQTTV5Packet.h>
/* Main deamon */
#include <error_handler.h>
#include <cert.h>
/* UART deamon */
#include <uart_deamon.h>

extern SIM_intf *sim;
extern struct mqtt_client client;
static mqtt_deamon_handler *mdh;

static SemaphoreHandle_t mutex;
static StaticSemaphore_t xMutexBuffer;

/* Write to broker */
int mqtt_tls_write(unsigned char *buf, size_t len)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    int len = mbedtls_ssl_write(&mdh->ctx.ssl_ctx, buf, len);
    xSemaphoreGive(mutex);
    return len;
}

static int mqtt_packet_ack(mqtt_deamon_handler *handler, unsigned char *buf, size_t len, MQTTProperty *packet_num_prop)
{
    /* Create name */
    MQTTString topicString = MQTTString_initializer;
    char *channel_name = (char *)malloc(sizeof(char) * (strlen(handler->username) + 2));
    memcpy(channel_name, handler->username, strlen(handler->username));
    channel_name[strlen(channel_name)] = 'i';
    topicString.cstring = channel_name;

    /* Set property */
    MQTTProperty ack_properties_array[1];
    MQTTProperties ack_properties = MQTTProperties_initializer;
    ack_properties.count = 1;
    ack_properties.array = ack_properties_array;
    ack_properties_array[0] = *packet_num_prop;

    /* Send ACK */
    int len = MQTTV5Serialize_publish(buf, len, 0, QOS, 0, 0, topicString, &ack_properties, NULL, 0;)
        mqtt_tls_write(recbuf, len);

    free(channel_name);
}

/* Get exactly len bytes of data from socket */
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
    uint8_t sendbuf[MAIN_MQTT_SEND_BUF_SIZE];

    unsigned int msg_id = 1;
    unsigned char ch_num = 0;

    /* Used for properties reading */
    MQTTProperty properties_array[PROPERTIES_SIZE];
    MQTTProperties properties = MQTTProperties_initializer;
    properties.array = properties_array;
    properties.max_count = PROPERTIES_SIZE;

    while (1)
    {
        /* Wait untill socket informs you that you got a new packet */
        xQueueReceive(handler->queue, &err, portMAX_DELAY);

        /* Check if sockt didn't get any error */
        if (err == SIM_closed || err == SIM_closedOk)
        {
            handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
            break;
        }

        /* Check if ping needed */
        // if ()

        /* Flush queue if everything was already processed */
        if (sim->tcp_cmds[(SIM_con_num)handler->ctx.net_ctx.fd].cmd->resp.data_len == 0)
        {
            xQueueReset(handler->queue);
            continue;
        }

        if ((err = MQTTV5Packet_readnb(recvbuf, MAIN_MQTT_REC_BUF_SIZE, &handler->trp)) >= 0)
        {
            switch (err)
            {
            case 0: /* timed out reading packet */
                break;
            case CONNACK:
            {
                /* Check if connected */
                unsigned char sessionPresent, connack_rc;

                if (MQTTV5Deserialize_connack(&properties, &sessionPresent, &connack_rc, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1 || connack_rc != 0)
                {
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, connack_rc);
                    goto exit;
                }

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
            }
            case SUBACK:
            {
                /* Check if last subscribe ok */
                unsigned short submsg_id;
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
            }
            case PUBLISH:
            {
                unsigned char dup;
                unsigned char qos_;
                unsigned char retained;
                unsigned short packet_id;
                MQTTString topic_name;
                char *payload;
                char payload_len;

                if (MQTTV5Deserialize_publish(&dup, &qos_, &retained, &packet_id, &topic_name, &properties, &payload, &payload_len, recvbuf, MAIN_MQTT_REC_BUF_SIZE) != 1)
                {
                    handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, -1);
                    goto exit;
                }

                switch (topic_name.cstring[USERNAME_LEN - 1])
                {
                case 'i':
                {
                    break;
                }
                case 's':
                {
                    break;
                }
                case 'd':
                {
                    /* Find packet number */
                    bool num_found = false;
                    unsigned char data_num_prop;
                    for (unsigned char i = 0; i < properties.count; i++)
                    {
                        if (properties.array[i].identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
                        {
                            data_num_prop = i;
                            num_found = true;
                        }
                    }

                    if (num_found == false)
                    {
                        handler->error_handler(&handler->handler, "MQTT", ext_type_non_fatal, -1);
                    }
                    else
                    {
                        /* Send ACK */
                        mqtt_packet_ack(handler, sendbuf, MAIN_MQTT_SEND_BUF_SIZE, properties.array + data_num_prop);
                    }

                    /* Send data to UART */
                    handler->publish_callb(payload, payload_len);

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
        xTaskCreate(mqtt_deamon, "mqtt_deamon", 4096, handler, 3, &handler->handler);
    }
    return handler->handler;
}

TaskHandle_t mqtt_deamon_delete_task(mqtt_deamon_handler *handler)
{
    if (handler->handler)
    {
        vTaskDelete(handler->handler);
        handler->handler = NULL;
    }
    return handler->handler;
}

int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      const char *server,
                      const char *port,
                      // const char *client_id,
                      const char *username,
                      const char *password,
                      const unsigned char **chain,
                      void (*socket_resp_handler)(int *err))
{
    int err;
    /* Prepare mutex */
    mutex = xSemaphoreCreateMutexStatic(&xMutexBuffer);
    /* Assign handler data to ptr */
    mdh = handler;
    /* Set username */
    handler->username = username;

    /* Init MQTT client */
    uint8_t sendbuf[MAIN_MQTT_SEND_BUF_SIZE];
    handler->trp.sck = (void *)&handler->ctx.ssl_ctx; // set contrxt
    handler->trp.getfn = mqtt_tls_read;               // set read function
    handler->trp.state = 0;

    /* TLS structures */
    handler->queue = xQueueCreate(10, sizeof(int));

    /* Open encrypted socket */
    if ((err = open_nb_socket(&handler->ctx, server, port, chain)) != 0)
    {
        return err;
    }
    // set handler that awakes deamon when new message comes
    if ((err = socket_set_handler(&handler->ctx, socket_resp_handler)) != 0)
    {
        return err;
    }
    /* Save ctx */

    /* Start deamon */
    mqtt_deamon_create_task(handler);
    if (!handler->handler)
    {
        /* Couldn't start deamon */
        return -1;
    }

    /* Connection parameters */
    MQTTV5Packet_connectData connect_data = MQTTV5Packet_connectData_initializer;
    connect_data.username.cstring = username;
    connect_data.password.cstring = password;
    connect_data.MQTTVersion = 5;
    connect_data.keepAliveInterval = 60;
    /* Connection properties, none needed */
    MQTTProperties conn_properties = MQTTProperties_initializer;

    int len = MQTTV5Serialize_connect(sendbuf, MAIN_MQTT_SEND_BUF_SIZE, &connect_data, &conn_properties);

    /* Connect to broker */
    if (mqtt_tls_write(sendbuf, len) == len)
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