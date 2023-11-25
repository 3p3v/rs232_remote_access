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
#include <esp_err.h>
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
/* MQTT-C lib */
// #include <mqtt.h>
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

static mqtt_deamon_handler mqtt_handler;
static sim_deamon_handler sim_handler;
static uart_deamon_handler uart_handler;

/* Function decalrations */
void *client_refresher(void *client);
void rec_callback(unsigned char * buf, unsigned int);
void main_task(void*);
void err_handling_task(void*);
void return_error(const char *tag, int err);
void mqtt_deamon(void *);
void socket_resp_handler(int *err);

/* Main */
void app_main(void)
{
    /* Run startup */
    xTaskCreate(main_task, "mqtt_main_task", 60000, NULL, 3, NULL);
}

void main_task(void*)
{
    int err;

    mqtt_handler.handler = NULL;
    mqtt_handler.queue = NULL;
    mqtt_handler.publish_callb = uart_write;
    mqtt_handler.error_handler = ext_error_send;

    sim_handler.handler = NULL;
    sim_handler.error_handler = ext_error_send;
    sim = &sim_handler.sim;

    uart_handler.handler = NULL;
    uart_handler.queue = NULL;
    uart_handler.uart_conf = uart_deamon_load_config();
    uart_handler.resp_handler = rec_callback;
    uart_handler.error_handler = ext_error_send;

    /* Init needed structures */
    cert_load_chain();
    auth_load();
    error_create_queue();

    /* Start error handing task */
    if (xTaskCreate(err_handling_task, "err_handling_task", 2048, NULL, 3, NULL) != pdPASS)
    {
        ESP_LOGE("ERROR_DEAMON", "COUNDN'T START, REBOOTING");
        esp_restart();

    }

    /* Start SIM deamon */
    if ((err = sim_deamon_start(&sim_handler)))
    {
        ESP_LOGE("SIM_DEAMON", "COUNDN'T START, REBOOTING");
        esp_restart();
    }
    /* Start UART deamon */
    if ((err = uart_deamon_start(&uart_handler)))
    {
        ESP_LOGE("UART_DEAMON", "COUNDN'T START, REBOOTING");
        esp_restart();
    }

    /* Start mqtt deamon */
    if ((err = mqtt_deamon_start(&mqtt_handler,
                                 &uart_handler.uart_conf,
                                 *auth_get_server(),
                                 *auth_get_port(),
                                 *auth_get_username(),
                                 *auth_get_password(),
                                 *cert_get_chain(),
                                 socket_resp_handler)))
    {
        ESP_LOGE("MQTT_DEAMON", "COUNDN'T START, REBOOTING");
        esp_restart();
    }
    cert_free_chain();

    /* OK */
    vTaskDelete(NULL);
}

void err_handling_task(void*)
{
    ext_error ext_err;
    int err;
    QueueHandle_t *main_queue = error_get_queue();

    while (1)
    {
        printf("START READING ERRORS\r\n");
        xQueueReceive(*main_queue, &ext_err, portMAX_DELAY);
        printf("GOT ERROR\r\n");

        if (sim_handler.handler == ext_err.handler)
        {
            echo_error(&ext_err);
            /* Unhandlable exception occured in SIM task */
            if (ext_err.type == ext_type_fatal)
            {
                goto exit;
            }
            else
            {
                ext_mqtt_send_error(&ext_err);
            }
        }
        else if (mqtt_handler.handler == ext_err.handler)
        {
            echo_error(&ext_err);
            /* Unhandlable exception occured in MQTT task, reset this task */
            if (ext_err.type == ext_type_fatal)
            {
                mqtt_deamon_stop(&mqtt_handler);
                if ((err = mqtt_deamon_start(&mqtt_handler,
                                             &uart_handler.uart_conf,
                                             *auth_get_server(),
                                             *auth_get_port(),
                                             *auth_get_username(),
                                             *auth_get_password(),
                                             *cert_get_chain(),
                                             socket_resp_handler)))
                {
                    ESP_LOGE("MQTT_DEAMON", "COUNDN'T START, REBOOTING");
                    cert_free_chain();
                    goto exit;
                }
                else
                {
                    cert_free_chain();
                    ext_mqtt_send_error(&ext_err);
                }
            }
        }
        else if (uart_handler.handler == ext_err.handler)
        {
            echo_error(&ext_err);
            /* Unhandlable exception occured in MQTT task, reset this task */
            if (ext_err.type == ext_type_fatal)
            {
                uart_deamon_stop(&uart_handler);
                if ((err = uart_deamon_start(&uart_handler)))
                {
                    ESP_LOGE("UART_DEAMON", "COUNDN'T START, REBOOTING");
                    goto exit;
                }
            }
            else
            {
                ext_mqtt_send_error(&ext_err);
            }
        }
        else if (ext_err.handler)

        free(ext_err.module);
    }

exit:
    free(ext_err.module); // TODO
    sim_deamon_stop(&sim_handler);
    mqtt_deamon_stop(&mqtt_handler);
    uart_deamon_delete_task(&uart_handler);
    error_delete_queue();
    auth_free_username();
    auth_free_password();
    auth_free_server();
    auth_free_port();

    esp_restart();
}

void rec_callback(unsigned char * buf, unsigned int len)
{
    int err;
    if ((err = mqtt_write_d(&mqtt_handler, buf, len)) <= 0)
    {
        ext_error_send(&mqtt_handler, "MQTT", ext_type_fatal, err);
    }
}

void socket_resp_handler(int *err)
{
    mqtt_deamon_awake(&mqtt_handler, err);
}