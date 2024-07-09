#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <cmd_defs.h>

#define MQTT_DAEMON_TASK_NAME "MQTT_DAEMON"
#define TAG MQTT_DAEMON_TASK_NAME

/* Buffers based on packet sizes */
#define MAIN_MQTT_REC_BUF_SIZE 1460
#define MAIN_MQTT_SEND_BUF_SIZE 1460

#define SEC_IN_MIN 60
#define PING_TIMER_RES 1000000
#define QOS 0
#define PROPERTIES_SIZE 5

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
    SemaphoreHandle_t send_buf_mutex_handle;
    unsigned char sendbuf[MAIN_MQTT_SEND_BUF_SIZE];
    /* data msgs */
    SemaphoreHandle_t send_msgs_mutex_handle;
    mqtt_msg send_msgs[MAX_SAVED];
    SemaphoreHandle_t mbed_mutex_handle;
    MQTTV5Transport trp;
    uart_config_t *uart_conf;
    volatile bool initialized;
    /* Number of next msg to send */
    volatile char slave_num;
    /* Number of next message from master */
    volatile char master_num;
    /* Number of not ack */
    unsigned char m_not_ack;
    volatile unsigned char s_not_ack;
    /* Master asked for a new session */
    bool m_clean_session;// = false;
    int keep_alive_int;
    /* MQTT message id */
    unsigned short msg_id;
    unsigned char qos;
    /**/
    QueueHandle_t queue;
    /* MbedTLS encryption context */
    mbedtls_context ctx;
    const char *username;
    void (*error_handler)(void *, const char *, int , int );
    int (*publish_callb)(unsigned char *, size_t);
    size_t (*buf_check)(mbedtls_context *ctx);
} mqtt_deamon_handler;

/* Start deamon */
int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      uart_config_t *uart_conf,
                      char *server,
                      char *port,
                      char *username,
                      char *password,
                      unsigned char (*get_cert)(unsigned char),
                      unsigned char chain_size);

/* Kill deamon */
int mqtt_deamon_stop(mqtt_deamon_handler *handler);