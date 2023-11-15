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
#include <mqtt.h>
/* Main deamon */
#include <error_handler.h>
#include <cert.h>

extern SIM_intf *sim;
extern struct mqtt_client client;

// static mqtt_deamon_handler *mqtt_deamon_get_handler()
// {
//     static QueueHandle_t queue;
//     return &queue;
// }

// mqtt_deamon_handler *mqtt_deamon_set_handle(void (*error_handler)(const char *, int), void (*hard_error_handler)(const char *, int))
// {
//     mqtt_deamon_handler *handler = mqtt_deamon_get_handle();
//     handler->error_handler = error_handler;
//     handler->hard_error_handler = hard_error_handler;
//     handler->queue = xQueueCreate(1, sizeof(int));
//     // handler->resp_handler = resp_handler;
//     return handler;
// }

// mqtt_deamon_handler *mqtt_deamon_free_handle()
// {
//     mqtt_deamon_handler *handler = mqtt_deamon_get_handle();
//     handler->error_handler = NULL;
//     handler->hard_error_handler = NULL;
//     vQueueDelete(handler->queue);
//     // handler->resp_handler = resp_handler;
//     return handler;
// }

// static mqtt_deamon_handler *mqtt_deamon_get_handle()
// {
//     static mqtt_deamon_handler handle;
//     return &handle;
// }

// static QueueHandle_t *mqtt_deamon_create_queue()
// {
//     mqtt_deamon_handler *handler = mqtt_deamon_get_handle();

//     return &handler->queue;
// }

// static QueueHandle_t *mqtt_deamon_get_queue()
// {
//     static QueueHandle_t queue;
//     return &queue;
// }

// static void mqtt_deamon_delete_queue()
// {
//     mqtt_deamon_handler *handler = mqtt_deamon_get_handle();

// }

void mqtt_deamon(void *v_mqtt_deamon_handler)
{
    int err = SIM_noErrCode;
    mqtt_deamon_handler *handler = (mqtt_deamon_handler *)v_mqtt_deamon_handler;

    while (1)
    {
        /* Wait untill socket informs you that you got a new packet */
        xQueueReceive(handler->queue, &err, 100000U / portTICK_PERIOD_MS);

        /* Check if sockt didn't get any error */
        if (err == SIM_closed || err == SIM_closedOk)
        {
            handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
            break;
        }
        
        /* Flush queue if everything was already processed */
        if (sim->tcp_cmds[(SIM_con_num)handler->socket_num].cmd->resp.data_len == 0)
        {
            xQueueReset(handler->queue);
            continue;
        }

        // TODO exec only if there is any data, if there is not, then reset queue
        err = mqtt_sync(&handler->client);
        if ((err) < 0)
        {
            printf("MQTT error: %i\r\n", err);
            
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

TaskHandle_t mqtt_deamon_create_task(mqtt_deamon_handler *handler)
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
    static bool reconnect = false; 
    int err;
    /* TLS and MQTT specyfic structures */
    // mqtt_deamon_set_handle(error_handler, hard_error_handler);
    mbedtls_context ctx;
    handler->queue = xQueueCreate(10, sizeof(int));

    /* Load certyficates */
    // const unsigned char *ca_chain[] = {(const unsigned char *)SERVER_IM2_CERT,
    //                                    (const unsigned char *)SERVER_IM1_CERT,
    //                                    (const unsigned char *)SERVER_ROOT_CERT,
    //                                    NULL};

    /* Open encrypted socket */
    err = open_nb_socket(&ctx, server, port, chain);
    handler->socket_num = ctx.net_ctx.fd;
    err = socket_set_handler(&ctx, socket_resp_handler); // set handler that awakes deamon when new message comes
    mqtt_pal_socket_handle sockfd = &ctx.ssl_ctx;
    if (sockfd == NULL) // TODO change
    {
        // handler->error_handler(&handler->handler, "MBEDTLS", ext_type_fatal, err);
        return err;
    }

    /* Init MQTT client */
    static uint8_t sendbuf[MAIN_MQTT_SEND_BUF_SIZE];
    static uint8_t recvbuf[MAIN_MQTT_REC_BUF_SIZE];
    const char *client_id = NULL;
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    mqtt_init(&handler->client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), handler->publish_callback);
    /* Create connect message */
    /* Ensure having a clean session as client id is not specyfied */
    mqtt_connect(&handler->client, client_id, NULL, NULL, 0, username, password, connect_flags, 60);
    if (handler->client.error != MQTT_OK)
    {
        // handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, err);
        return handler->client.error;
    }
    // mqtt_sync(&handler->client);

    // mqtt_publish(&handler->client, "console_device1", "t", 1, 0);
    mqtt_subscribe(&handler->client, "console_device1", 0);
    // mqtt_sync(&handler->client);
    mqtt_deamon_awake(handler, &handler->client.error);

    /* Finally run the deamon */
    mqtt_deamon_create_task(handler);
    if (handler->handler)
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
    mbedtls_net_context ctx = {.fd = handler->socket_num};

    if (!(handler->handler))
    {
        // TODO close socket, clean up
        close_nb_socket(&ctx);      
        if (ctx.fd == -1)
            return 0;
        else
            return -2;
    }
    else
    {
        return -1;
    }
}