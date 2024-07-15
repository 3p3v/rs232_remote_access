#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <cmd_defs.h>
#include <MQTTV5Packet.h>
#include <freertos/semphr.h>
#include <driver/uart.h>
#include <driver/gptimer.h>
#include <mbedtls_sockets.h>
#include <AT_socket.h>

#define MQTT_DAEMON_TASK_NAME "MQTT_DAEMON"
#define TAG MQTT_DAEMON_TASK_NAME
#define MQTT_DAEMON_STACK_SIZE 20000
#define MQTT_DAEMON_TASK_PRIORITY 3

/* Buffers based on packet sizes */
#define MAIN_MQTT_REC_BUF_SIZE 1460
#define MAIN_MQTT_SEND_BUF_SIZE 1460

#define SEC_IN_MIN 60
#define PING_TIMER_RES 1000000
#define QOS 0
#define PROPERTIES_SIZE 5

#define MQTT_DEAMON_DEF_PING_OFFSET 5
#define MQTT_DEAMON_DEF_PING_TIME 60 - MQTT_DEAMON_DEF_PING_OFFSET

typedef enum
{
    mqtt_daemon_ok = 0,
    mqtt_daemon_uerr = -128, // unknown error
    mqtt_daemon_werr,        // write error
    mqtt_daemon_rerr,        // read error
    mqtt_daemon_packet_not_found,
    mqtt_daemon_structure_not_inited,
    mqtt_deamon_disconnected,
    mqtt_daemon_subscribe_err,
    mqtt_daemon_data_too_long,
    mqtt_daemon_serialize_error,
    mqtt_daemon_deserialize_error,
    mqtt_daemon_connect_err,
    mqtt_daemon_socket_err
} mqtt_daemon_code;

typedef struct mqtt_msg
{
    bool ack;
    char num;
    unsigned char data[MAX_SLAVE_PACKET_LEN];
    size_t len;
} mqtt_msg;

typedef struct mqtt_deamon_handler
{
    TaskHandle_t handler;
    volatile bool initialized;

    /* Forwarding data to UART */
    void *uart_handler;
    void (*uart_change_conf)(void *uart_handler, uart_sett sett, void *arg);
    int (*publish_callb)(unsigned char *, size_t);

    /* Mutexes */
    SemaphoreHandle_t send_msgs_mutex_handle;
    SemaphoreHandle_t send_buf_mutex_handle;
    SemaphoreHandle_t mbed_mutex_handle;

    /* MQTT messaging */
    MQTTV5Transport trp;
    unsigned short msg_id;
    unsigned char qos;
    unsigned char sendbuf[MAIN_MQTT_SEND_BUF_SIZE];
    const char *username;

    /* Data flow */
    mqtt_msg send_msgs[MAX_SAVED];
    volatile char slave_num;          /* ID of next msg to send */
    volatile char master_num;         /* ID of next message from master */
    unsigned char m_not_ack;          /* Number of messages not acknowledged by slave */
    volatile unsigned char s_not_ack; /* Number of messages not acknowledged by master */

    /* Ping indicator */
    gptimer_handle_t ping_timer;
    int keep_alive_int; /* Keep alive interval */

    /* Task messaging */
    QueueHandle_t queue;

    /* MbedTLS encryption context */
    mbedtls_context ctx;
    AT_socket_context socket_ctx; /* Low-level SIM driver */

    /* Error forwarding */
    void (*error_handler)(void *, const char *, int, int);
} mqtt_deamon_handler;

void mqtt_daemon_reinit(
    mqtt_deamon_handler *handler,
    void *uart_handler,
    AT_socket_context socket_ctx,
    int (*publish_callb)(unsigned char *, size_t),
    void (*uart_change_conf)(void *uart_handler, uart_sett sett, void *arg),
    void (*error_handler)(void *, const char *, int, int));

/* Start deamon */
mqtt_daemon_code mqtt_deamon_start(
    mqtt_deamon_handler *handler,
    char *server,
    char *port,
    char *username,
    char *password,
    unsigned char (*get_cert)(unsigned char),
    unsigned char chain_size);

/* Kill deamon */
int mqtt_deamon_stop(mqtt_deamon_handler *handler);