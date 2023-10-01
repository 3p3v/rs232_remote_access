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
#include <mqtt.h>
/* Main deamon */
#include <error_handler.h>
#include <mqtt_deamon.h>
#include <sim_deamon.h>
#include <uart_deamon.h>
#include <auth.h>
#include <ctrl_ch.h>
#include <info_ch.h>
/* TEMPORARY */
#include <cert.h>

/* Definitions */
#define MAIN_SIM_BUF_SIZE 12000

#define MQTT_TOPIC "console_device1"

/* Global variables */
SIM_intf *sim;
struct mqtt_client *client;
static mqtt_deamon_handler *mqtt_d_handler;

/* Function decalrations */
void *client_refresher(void *client);
void publish_callback(void **unused, struct mqtt_response_publish *published);
void rec_callback(const unsigned char *, unsigned int);
void main_task(void);
void return_error(const char *tag, int err);
void mqtt_deamon(void *);
void socket_resp_handler(int *err);

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
    xTaskCreate(&main_task, "mqtt_main_task", 35000, NULL, 3, NULL);

    // while(1);
}

void main_task(void)
{
    ext_error ext_err;
    int err;

    mqtt_deamon_handler mqtt_handler = {.handler = NULL,
                                        .queue = NULL,
                                        .publish_callback = publish_callback,
                                        .error_handler = ext_error_send};
    mqtt_d_handler = &mqtt_handler;
    client = &mqtt_handler.client;

    sim_deamon_handler sim_handler = {.handler = NULL,
                                      .error_handler = ext_error_send};
    sim = &sim_handler.sim;

    uart_deamon_handler uart_handler = {.handler = NULL,
                                        .queue = NULL,
                                        .uart_conf = uart_deamon_load_config(),
                                        .resp_handler = rec_callback,
                                        .error_handler = ext_error_send};

    /* Init needed structures */
    QueueHandle_t *main_queue = error_create_queue();
    cert_load_chain();
    auth_load();

    /* Enable the persistant ESP32 memory */
    if ((err = nvs_flash_init()))
    {
        // mqtt_deamon_delete_queue();
        // error_delete_queue();
        return_error("nvs_flash init", err);
        goto exit;
    }

    /* Start SIM deamon */
    if ((err = sim_deamon_start(&sim_handler)))
    {
        // mqtt_deamon_delete_queue();
        // error_delete_queue();
        return_error("sim_deamon startup", err);
        goto exit;
    }
    ESP_LOGI("DEAMON", "6");
    /* Start UART deamon */
    if ((err = uart_deamon_start(&uart_handler)))
    {
        // mqtt_deamon_delete_queue();
        // error_delete_queue();
        return_error("mqtt_deamon startup", err);
        goto exit;
    }

    /* Start mqtt deamon */
    if ((err = mqtt_deamon_start(&mqtt_handler,
                                 *auth_get_server(),
                                 *auth_get_port(),
                                 *auth_get_username(),
                                 *auth_get_password(),
                                 *cert_get_chain(),
                                 socket_resp_handler)))
    {
        // mqtt_deamon_delete_queue();
        // error_delete_queue();
        return_error("mqtt_deamon startup", err);
        goto exit;
    }

    while (1)
    {
        xQueueReceive(*main_queue, &ext_err, portMAX_DELAY);

        if (sim_handler.handler == ext_err.handler)
        {
            const char *deamon_name = "SIM deamon";
            echo_error(deamon_name, &ext_err);
            /* Unhandlable exception occured in SIM task */
            if (ext_err.type == ext_type_fatal)
            {
                goto exit;
            }
            else
            {
                ext_mqtt_send_error(deamon_name, &ext_err);
            }
            // break;
        }
        else if (mqtt_handler.handler == ext_err.handler)
        {
            const char *deamon_name = "MQTT deamon";
            echo_error(deamon_name, &ext_err);
            /* Unhandlable exception occured in MQTT task, reset this task */
            if (ext_err.type == ext_type_fatal)
            {
                mqtt_deamon_delete_task(&mqtt_handler);
                if ((err = mqtt_deamon_start(&mqtt_handler,
                                            *auth_get_server(),
                                            *auth_get_port(),
                                            *auth_get_username(),
                                            *auth_get_password(),
                                            *cert_get_chain(),
                                            socket_resp_handler)))
                {
                    return_error("mqtt_deamon startup", err);
                    goto exit;
                }
                else
                {
                   ext_mqtt_send_error(deamon_name, &ext_err);
                }
            }
        }
        else if (uart_handler.handler == ext_err.handler)
        {
            const char *deamon_name = "UART deamon";
            echo_error(deamon_name, &ext_err);
            /* Unhandlable exception occured in MQTT task, reset this task */
            if (ext_err.type == ext_type_fatal)
            {
                uart_deamon_delete_task(&uart_handler);
                if ((err = uart_deamon_start(&uart_handler)))
                {
                    return_error("uart_deamon startup", err);
                    goto exit;
                }
            }
            else
            {
                ext_mqtt_send_error(deamon_name, &ext_err);
            }
        }

        free(ext_err.module);
    }

    

exit:
    free(ext_err.module);
    sim_deamon_delete_task(&sim_handler);
    mqtt_deamon_delete_task(&mqtt_handler);
    uart_deamon_delete_task(&uart_handler);
    error_delete_queue();
    cert_free_chain();
    auth_free_username();
    auth_free_password();
    auth_free_server();
    auth_free_port();

    reboot();
    // mqtt_publish(&client, MQTT_TOPIC, "test", strlen("test") + 1, MQTT_PUBLISH_QOS_0);
    // if (client.error != MQTT_OK)
    // {
    //     return_error("MQTT-C", client.error);
    // }
    // xQueueSend(client_queue, &client.error, portMAX_DELAY);

    // mqtt_deamon_delete_queue();
    // error_delete_queue();
    // abort();
    // vTaskDelete(NULL);
}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{
    char *topic_name = (char *)malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("GOT MSG IN TOPIC: %s.", topic_name);
    /* Control channel handling */
    if (strstr(topic_name, "control"))
    {
        ctrl_ch_handler((const char *)published->application_message);
    }
    /* Information channel handling */
    else
    {
        info_ch_handler((const char *)published->application_message);
    }
}

void rec_callback(const unsigned char *, unsigned int)
{
}

void socket_resp_handler(int *err)
{
    mqtt_deamon_awake(mqtt_d_handler, err);
}