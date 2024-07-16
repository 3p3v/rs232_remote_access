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
/* Daemons */
#include <error_handler.h>
#include <mqtt_deamon.h>
#include <mqtt_ch_rw.h>
#include <sim_deamon.h>
#include <uart_deamon.h>
#include <auth.h>
#include <info_ch.h>
/* TEMPORARY */
#include <cert.h>

/* Definitions */
#define ERROR_HANDLING_TASK_NAME "ERROR_DAEMON"
#define ERROR_HANDLING_TASK_STACK_SIZE 2048
#define ERROR_HANDLING_TASK_PRIORITY 3

#define MAIN_TASK_NAME "MAIN_TASK"
#define MAIN_TASK_STACK_SIZE 20000
#define MAIN_TASK_PRIORITY 3

/* Global variables */
SIM_intf *sim;
struct mqtt_client *client;

mqtt_deamon_handler mqtt_handler;
static sim_deamon_handler sim_handler;
static uart_deamon_handler uart_handler;

auth_pack auth;

/* Function decalrations */
void rec_callback(mqtt_deamon_handler *handler, unsigned char *buf, unsigned int len);

/* Device start task */
void main_task(void *);

/* Task from handling errors */
void err_handling_task(void *);

/* Start error handling task */
int error_handler_start();

/* Print device name */
void printf_dev_name(const char *name);

/* Print error */
void log_error(const char *module, const char *info, int reason);

/* Reset device */
void reboot();

/* Main */
void app_main(void)
{
    /* Run startup */
    xTaskCreate(main_task, MAIN_TASK_NAME, MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL);
}

void main_task(void *)
{
    int err;
    
    /* Load credentials from memory */
    if ((err = auth_load(&auth)))
    {
        log_error(AUTH_TASK_NAME, "COUNDN'T LOAD, REBOOTING", err);
        reboot();
    }
    error_create_queue();

    /* Cout device name */
    printf_dev_name(auth.username);

    /* Start error handing task */
    if ((err = error_handler_start(&auth)))
    {
        log_error(ERROR_HANDLER_TASK_NAME, "COUNDN'T START, REBOOTING", err);
        reboot();
    }

    /* Start SIM deamon */
    sim_daemon_reinit(
        &sim_handler,
        &ext_error_send);
    sim = &sim_handler.sim;

    if ((err = sim_deamon_start(&sim_handler, auth.apn, auth.username, auth.password)))
    {
        log_error(SIM_DAEMON_TASK_NAME, "COUNDN'T START, REBOOTING", err);
        reboot();
    }

    /* Start UART deamon */
    uart_deamon_reinit(
        &uart_handler,
        &mqtt_handler,
        &rec_callback,
        &ext_error_send);

    if ((err = uart_deamon_start(&uart_handler)))
    {
        log_error(UART_DAEMON_TASK_NAME, "COUNDN'T START, REBOOTING", err);
        reboot();
    }

    /* Start mqtt deamon */
    mqtt_daemon_reinit(
        &mqtt_handler,
        &uart_handler,
        AT_socket_context_init(&sim_handler.sim),
        &uart_write,
        &uart_change_conf,
        &uart_get_conf,
        &ext_error_send);

    if ((err = mqtt_deamon_start(
            &mqtt_handler,
            auth.server,
            auth.port,
            auth.username,
            auth.password,
            &cert_load_chain,
            auth.chain_size)))
    {
        log_error(MQTT_DAEMON_TASK_NAME, "COUNDN'T START, REBOOTING", err);
        reboot();
    }

    /* OK */
    vTaskDelete(NULL);
}

void err_handling_task(void *)
{
    ext_error ext_err;
    int err;
    QueueHandle_t *main_queue = error_get_queue();

    while (1)
    {
        xQueueReceive(*main_queue, &ext_err, portMAX_DELAY);

        if (ext_err.type != ext_type_fatal)
        {
            if (sim_handler.handler == ext_err.handler)
            {
                ext_mqtt_send_error(SIM_DAEMON_TASK_NAME, &ext_err);
                echo_error(SIM_DAEMON_TASK_NAME, &ext_err);
            }
            else if (mqtt_handler.handler == ext_err.handler)
            {
                ext_mqtt_send_error(MQTT_DAEMON_TASK_NAME, &ext_err);
                echo_error(MQTT_DAEMON_TASK_NAME, &ext_err);
            }
            else if (uart_handler.handler == ext_err.handler)
            {
                ext_mqtt_send_error(UART_DAEMON_TASK_NAME, &ext_err);
                echo_error(UART_DAEMON_TASK_NAME, &ext_err);
            }
        }
        else
        {
            break;
        }
    }

    sim_deamon_stop(&sim_handler);
    mqtt_deamon_stop(&mqtt_handler);
    uart_deamon_delete_task(&uart_handler);
    error_delete_queue();
    auth_free(&auth);

    reboot();
}

int error_handler_start()
{
    if (xTaskCreate(err_handling_task, ERROR_HANDLING_TASK_NAME, ERROR_HANDLING_TASK_STACK_SIZE, NULL, ERROR_HANDLING_TASK_PRIORITY, NULL) == pdPASS)
        return 0;
    else
        return -1;
}

void rec_callback(mqtt_deamon_handler *handler, unsigned char *buf, unsigned int len)
{
    int err;

    if ((err = mqtt_transmit(handler, buf, len)) <= 0)
    {
        ext_error_send(handler, MQTT_DAEMON_TASK_NAME, ext_type_fatal, err);
    }
}

void printf_dev_name(const char *name)
{
    printf("\r\n");
    printf("/****************************************************************************/\r\n");
    printf("/*****************/");
    printf("DEVICE MAC ADDRESS IS: %s", name);
    printf("/*****************/\r\n");
    printf("/****************************************************************************/\r\n");
}

void log_error(const char *module, const char *info, int reason)
{
    ESP_LOGE(module, "%s.\nReason: %i", info, reason);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
}

void reboot()
{
    esp_restart();
}