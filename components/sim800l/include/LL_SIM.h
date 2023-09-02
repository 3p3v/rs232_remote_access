#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "SIM_common_def.h"
// #include "SIM_common.h"

/* UART config */
#define LL_SIM_DEF_UART_NUM UART_NUM_1
#define LL_SIM_DEF_UART_TX GPIO_NUM_17
#define LL_SIM_DEF_UART_RX GPIO_NUM_16
#define LL_SIM_DEF_UART_BAUD_RATE 9600

/* Sleep and reset pins */
#define LL_SIM_DEF_DRT_PIN GPIO_NUM_23
#define LL_SIM_DEF_RST_PIN GPIO_NUM_18

/* Interrupt config */
// #define LL_SIM_DEF_DOWNLOAD_TIME 10000
#define LL_SIM_DEF_QUEUE_SIZE 10
#define LL_SIM_DEF_PATTERN_QUEUE_SIZE 5
#define LL_SIM_PATTERN_INTERVAL 0
#define LL_SIM_MIN_POST_IDLE 0
#define LL_SIM_MIN_PRE_IDLE 0

/* Buffers */
#define LL_SIM_DEF_BUF_SIZE 240
#define LL_SIM_DEF_TX_BUF_SIZE 240

typedef struct LL_SIM_int
{
    /* Must not be deleted */
    /* Buffer used for receiving raw messages from SIM800L module */
    char at[SIM_MAX_AT_STR_LEN];
    char *buf;// [LL_SIM_DEF_BUF_SIZE];
    unsigned int buf_len;
    /* Length of last received message from SIM800L module */
    unsigned int rec_len;
    // SIM_response resp;

    /* User defined, can be changed */
    gpio_num_t tx;
    gpio_num_t rx;
    int baudRate;
    gpio_num_t drt;
    gpio_num_t rst;
    uart_port_t uart;
    // uart_config_t uartConf;
    QueueHandle_t uartQueue;
} LL_SIM_int;

/* User defined */
/* Set SIM800L struct to default values */
void LL_SIM_def(LL_SIM_int* sim);
/* Initialize microcontrollers interfaces */
LL_SIM_error LL_SIM_init(const LL_SIM_int* sim);
/* Denitialize microcontrollers interfaces */
LL_SIM_error LL_SIM_deinit(const LL_SIM_int* sim);
// LL_SIM_error LL_SIM_reinit(const LL_SIM* interface);

/* User defined */
LL_SIM_error LL_SIM_sendData(const LL_SIM_int* sim, const char *data);
/* Receive raw response from SIM800l */
SIM_data_len LL_SIM_receiveRaw(const LL_SIM_int* sim, SIM_time time);
void LL_SIM_delay(int ms);

/* Set DRT pin high/low */
LL_SIM_error LL_SIM_setDRT(const LL_SIM_int* sim, LL_SIM_pin set);
/* Set RST pin high/low */
LL_SIM_error LL_SIM_setRST(const LL_SIM_int* sim, LL_SIM_pin set);


