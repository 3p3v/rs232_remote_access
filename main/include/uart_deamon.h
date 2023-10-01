#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <driver/uart.h>
#include <esp_log.h>

/* UART config, no changable */
#define UART_DEAMON_DEF_UART_NUM UART_NUM_2
#define UART_DEAMON_DEF_UART_TX GPIO_NUM_4
#define UART_DEAMON_DEF_UART_RX GPIO_NUM_2
/* UART config, rewritten during first change of parameters */
#define UART_DEAMON_DEF_UART_BAUD_RATE 9600
#define UART_DEAMON_DEF_UART_DATA_BITS UART_DATA_8_BITS
#define UART_DEAMON_DEF_UART_PARITY UART_PARITY_DISABLE
#define UART_DEAMON_DEF_UART_STOP_BITS UART_STOP_BITS_1
#define UART_DEAMON_DEF_UART_FLOW_CTRL UART_HW_FLOWCTRL_DISABLE
#define UART_DEAMON_DEF_UART_RX_FLOW_CTRL_TRESH 0
#define UART_DEAMON_DEF_UART_SOURCE_CLK UART_SCLK_DEFAULT
/* UART buffers, no changable */
#define UART_DEAMON_DEF_TX_BUF_SIZE 1000
#define UART_DEAMON_DEF_RX_BUF_SIZE 1000
#define UART_DEAMON_DEF_REC_BUF_SIZE 1000
/* UART interrupt config, no changable */
#define UART_DEAMON_DEF_QUEUE_SIZE 60

typedef struct uart_deamon_handler
{
    TaskHandle_t handler;
    QueueHandle_t queue;
    uart_config_t uart_conf;
    void (*resp_handler)(const unsigned char *, unsigned int);
    void (*error_handler)(void *handler, const char *module, int type, int err);
    // void (*hard_error_handler)(TaskHandle_t *);
} uart_deamon_handler;


// uart_deamon_handler *uart_deamon_set_handle(void (*resp_handler)(const unsigned char *, unsigned int), void (*error_handler)(const char *, int), void (*hard_error_handler)(const char *, int));

// static uart_deamon_handler *uart_deamon_get_handle();

void uart_deamon(void *v_handler);

TaskHandle_t uart_deamon_create_task(uart_deamon_handler *handler);

TaskHandle_t uart_deamon_delete_task(uart_deamon_handler *handler);

uart_config_t uart_deamon_load_config();

// static uart_config_t *uart_deamon_get_config();

// uart_config_t *uart_deamon_set_config(uart_config_t *new_uart_conf);

uart_config_t *uart_deamon_save_config(uart_config_t *new_uart_conf);

/* Start UART deamon.
 * If driver install and starting deamon was successful 
 * then handler != NULL and returned value == 0.
 * If driver install and uninstall was successful but starting deamon was't 
 * then handler == NULL and returned value == 0.
 * If both we unsuccessfull 
 * then handler == NULL and returned value != 0.*/
int uart_deamon_start(uart_deamon_handler *handler);

int uart_deamon_stop(uart_deamon_handler *handler);

// TaskHandle_t *uart_deamon_get_task();