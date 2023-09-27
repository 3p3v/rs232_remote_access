#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
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
#include <mqtt_deamon.h>
/* TEMPORARY */
#include <cert.h>

/* Definitions */
#define MAIN_SIM_BUF_SIZE 12000
#define MAIN_MQTT_REC_BUF_SIZE 1024
#define MAIN_MQTT_SEND_BUF_SIZE 1024

#define MAIN_USERNAME "admin"
#define MAIN_PASSWORD "admin"

#define WEB_SERVER "www.3p3v.pl"
#define WEB_PORT "8883"
#define MQTT_TOPIC "console_device1"

/* Global variables */
SIM_intf *sim;
struct mqtt_client client; /* TLS and MQTT specyfic structures */
// QueueHandle_t main_queue;

/* Function decalrations */
void *client_refresher(void *client);
void publish_callback(void **unused, struct mqtt_response_publish *published);
void main_task(void);
void return_error(const char *tag, int err);
void mqtt_deamon(void *);

/* Main */
void app_main(void)
{
    int err = ESP_OK;

    /* Set time in case it's needed to check a validity of certyficates */
    struct timeval tv_now;
    struct timezone t_z = {.tz_dsttime = 2, .tz_minuteswest = -39};
    gettimeofday(&tv_now, NULL);
    tv_now.tv_sec = 1694784830;
    settimeofday(&tv_now, NULL);

    /* Run main */
    xTaskCreate(main_task, "mqtt_main_task", 20000, NULL, 3, NULL);

    // while(1);
}

void main_task(void)
{
    TaskHandle_t handler;
    int err;
    QueueHandle_t main_queue = *error_get_queue();

    /* Enable the persistant ESP32 memory */
    if ((err = nvs_flash_init()))
    {
        mqtt_deamon_delete_queue();
        error_delete_queue();
        return_error("nvs_flash init", err);
    }

    /* Start SIM deamon */
    if ((err = sim_deamon_start(&handler)))
    {
        mqtt_deamon_delete_queue();
        error_delete_queue();
        return_error("sim_deamon startup", err);
    }

    /* Start UART deamon */
    // TODO

    /* Start mqtt deamon */
    if ((err = mqtt_deamon_start(&handler)))
    {
        mqtt_deamon_delete_queue();
        error_delete_queue();
        return_error("mqtt_deamon startup", err);
    }

    while (1)
    {
        xQueueReceive(main_queue, &handler, portMAX_DELAY);

        if (*sim_deamon_get_task() == handler)
        {
            /* Unhandlable exception occured in SIM task */
            sim_deamon_delete_task();
            mqtt_deamon_delete_task();
            mqtt_deamon_delete_queue();
            error_delete_queue();
            return_error("sim_deamon", err);
            // break;
        }
        else if (*mqtt_deamon_get_task() == handler)
        {
            /* Unhandlable exception occured in MQTT task, reset this task */
            echo_error("mqtt_deamon", err);
            mqtt_deamon_delete_task();
            if ((err = mqtt_deamon_start(&handler)))
            {
                return_error("mqtt_deamon startup", err);
            }
        }
    }

    // mqtt_publish(&client, MQTT_TOPIC, "test", strlen("test") + 1, MQTT_PUBLISH_QOS_0);
    // if (client.error != MQTT_OK)
    // {
    //     return_error("MQTT-C", client.error);
    // }
    // xQueueSend(client_queue, &client.error, portMAX_DELAY);
    
    mqtt_deamon_delete_queue();
    error_delete_queue();
    // abort();
    // vTaskDelete(NULL);
}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    /* Control channel handling */
    if (strstr(topic_name, "control"))
    {

    }
    /* Information channel handling */
    else
    {

    }
}






