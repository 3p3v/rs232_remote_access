#pragma once

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
#include <MQTT-C/include/mqtt.h>
/* Main deamon */
#include <error_handler.h>


extern SIM_intf *sim;
extern struct mqtt_client client;

QueueHandle_t *mqtt_deamon_get_queue()
{
    static QueueHandle_t queue = xQueueCreate(1, sizeof(int));
    return &queue;
}

void mqtt_deamon_delete_queue()
{
    QueueHandle_t *queue = mqtt_deamon_get_queue();
    vQueueDelete(*queue);
}

void mqtt_deamon(void *)
{
    SIM_error err = SIM_noErrCode;
    QueueHandle_t *queue = mqtt_deamon_get_queue();
    
    while (1)
    {
        xQueueReceive(*queue, &err, 100000U / portTICK_PERIOD_MS);
        // xQueueReceive((mqtt_task_grip *)grip->queue, &err, 100000U / portTICK_PERIOD_MS);
        if (err == SIM_closed || SIM_closedOk)
            break;
        
        mqtt_sync(&client);
        err = SIM_noErrCode;
    }

    /* Send error to main deamon */
    xQueueSend(*error_get_queue(), &xTaskGetCurrentTaskHandle(), portMAX_DELAY);

    /* wait for the task to be deleted */
    for (;;);
}

void mqtt_deamon_awake(int *err)
{
    QueueHandle_t *queue = mqtt_deamon_get_queue();
    xQueueSend(*queue, err, portMAX_DELAY);
}

TaskHandle_t *mqtt_deamon_get_task()
{
    static TaskHandle_t handle = NULL;
    return &handle;
}

TaskHandle_t mqtt_deamon_create_task()
{
    TaskHandle_t *handle = mqtt_deamon_get_task();
    if (!(*handle))
    {
        xTaskCreate(mqtt_deamon, "mqtt_deamon", 4096, NULL, 3, handle);
    }
    return *handle;
}

TaskHandle_t mqtt_deamon_delete_task()
{
    TaskHandle_t *handle = mqtt_deamon_get_task();
    if (*handle)
    {
        vTaskDelete(*handle);
    }
    return *handle;
}

int mqtt_deamon_start(TaskHandle_t *hander)
{
    int err;
    *handler = NULL;
    /* TLS and MQTT specyfic structures */
    mbedtls_context ctx;

    /* Load certyficates */
    const unsigned char *ca_chain[] = {(const unsigned char *)SERVER_IM2_CERT,
                                       (const unsigned char *)SERVER_IM1_CERT,
                                       (const unsigned char *)SERVER_ROOT_CERT,
                                       NULL};

    /* Open encrypted socket */
    err = open_nb_socket(&ctx, WEB_SERVER, WEB_PORT, ca_chain);
    err = socket_set_handler(&ctx, &mqtt_deamon_awake); // set handler that awakes deamon when new message comes
    mqtt_pal_socket_handle sockfd = &ctx.ssl_ctx;
    if (sockfd == NULL) //TODO change
    {
        echo_error("MbedTLS", err);
        return err;
    }

    /* Init MQTT client */
    static uint8_t sendbuf[MAIN_MQTT_SEND_BUF_SIZE]; 
    static uint8_t recvbuf[MAIN_MQTT_REC_BUF_SIZE]; 
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    /* Create connect message */
    const char *client_id = NULL;
    const char *username = MAIN_USERNAME;
    const char *password = MAIN_PASSWORD;
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION; /* Ensure having a clean session as client id is not specyfied */
    mqtt_connect(&client, client_id, NULL, NULL, 0, username, password, connect_flags, 60);
    if (client.error != MQTT_OK)
    {
        echo_error("MQTT-C", client.error);
        return client.error;
    }
    mqtt_deamon_awake(&client.error);
    
    /* Finally run the deamon */
    *handler = mqtt_deamon_create_task();
    if (*handler)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int mqtt_deamon_stop(TaskHandle_t *hander)
{
    *handler = mqtt_deamon_delete_task();
    
    if (!(*handler))
    {
        // TODO close socket, clean up
        return 0;
    }
    else
    {
        return -1;
    }
}