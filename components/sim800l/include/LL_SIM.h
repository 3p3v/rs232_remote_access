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
#define LL_SIM_DEF_TX_BUF_SIZE 240
// #define LL_SIM_DEF_BUF_SIZE 240

#define LL_SIM_DEF_CMDS_NUM 10
#define LL_SIM_DEF_CMD_QUEUE_SIZE  5


typedef struct SIM_cmd_grip
{
    SIM_cmd *cmd;
    QueueHandle_t queue;
} SIM_cmd_grip;

typedef struct LL_SIM_intf
{
    /* Must not be deleted */
    /* Buffer used for receiving raw messages from SIM800L module */
    char *buf;
    unsigned int buf_len;
    /* Length of last received message from SIM800L module */
    unsigned int rec_len;
    /*  */
    /* Number of not yet processed messages */
    unsigned char unread_num;
    SIM_cmd_grip cmds[LL_SIM_DEF_CMDS_NUM]; // [LL_SIM_DEF_BUF_SIZE];
    unsigned int cmds_num;
    /* Length of last received message from SIM800L module */
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
    SemaphoreHandle_t add_cmd_mutex;
    SemaphoreHandle_t write_mutex;
    SemaphoreHandle_t exec_mutex;
} LL_SIM_intf;
typedef LL_SIM_intf SIM_intf;

SIM_error SIM_exec(SIM_intf *sim);

/* User defined */
void LL_SIM_listen(LL_SIM_intf *sim);

void LL_SIM_receiveHandler(void *sim_void);

SIM_data_len LL_SIM_receiveRaw(LL_SIM_intf *sim);

LL_SIM_error LL_SIM_wait(LL_SIM_intf *sim, SIM_time time);
/* Set SIM800L struct to default values */
void LL_SIM_def(LL_SIM_intf *sim);
/* Initialize microcontrollers interfaces */
LL_SIM_error LL_SIM_init(const LL_SIM_intf *sim);
/* Denitialize microcontrollers interfaces */
LL_SIM_error LL_SIM_deinit(const LL_SIM_intf *sim);
// LL_SIM_error LL_SIM_reinit(const LL_SIM* interface);

/* User defined */
LL_SIM_error LL_SIM_sendData(const LL_SIM_intf *sim, const char *data, const unsigned int data_len);
/* Receive raw response from SIM800l */
// SIM_data_len LL_SIM_receiveRaw(const LL_SIM_intf* sim, SIM_time time);
// SIM_data_len LL_SIM_receiveRaw_checkCustomErr(const LL_SIM_intf *sim, SIM_time time, SIM_error (*check)(const LL_SIM_intf *, const SIM_err_pair *), const SIM_err_pair *err);
// SIM_data_len LL_SIM_receiveRaw_checkErr(const LL_SIM_intf *sim, SIM_time time, SIM_error (*check)(const LL_SIM_intf *));
void LL_SIM_delay(int ms);

/* Set DRT pin high/low */
LL_SIM_error LL_SIM_setDRT(const LL_SIM_intf *sim, LL_SIM_pin set);
/* Set RST pin high/low */
LL_SIM_error LL_SIM_setRST(const LL_SIM_intf *sim, LL_SIM_pin set);
