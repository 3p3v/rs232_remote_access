#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
/* timer */
#include <driver/gptimer.h>
/* MQTT-C lib */
#include <MQTTClient.h>
/**/
#include <mbedtls_sockets.h>
/* MQTT-C lib */
#include <MQTTV5Packet.h>
/* defs */
#include <cmd_defs.h>

/* Buffers based on packet sizes */
#define MAIN_MQTT_REC_BUF_SIZE 1460
#define MAIN_MQTT_SEND_BUF_SIZE 1460

/* Queue msgs */
#define PING_REQUEST 1000
#define CTS_CHANGED_STATE 998
#define RTS_CHANGED_STATE 999
#define MQTT_UNLOCK_T 1000

#define SEC_IN_MIN 60
#define PING_TIMER_RES 1000000
#define QOS 0
#define PROPERTIES_SIZE 5
#define USERNAME_LEN strlen("XX:XX:XX:XX:XX:XX")

/* RTS */
#define MQTT_RTS_PIN GPIO_NUM_21
/* CTS */
#define MQTT_CTS_PIN GPIO_NUM_19

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
    gptimer_handle_t ping_timer;
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
    // volatile mqtt_flow_ctrl flow_ctrl;
    // volatile bool cts;
    /* MQTT message id */
    unsigned short msg_id;
    unsigned char qos;
    /* RTS & CTS pins */
    // gpio_num_t rts;
    // gpio_num_t cts;
    /**/
    QueueHandle_t queue;
    /* MbedTLS encryption context */
    mbedtls_context ctx;
    char *username;
    // void (*publish_callback)(void **, struct mqtt_response_publish *);
    void (*error_handler)(void *, const char *, int , int );
    int (*publish_callb)(unsigned char *, size_t);
    // void (*hard_error_handler)(TaskHandle_t *);
} mqtt_deamon_handler;

void set_mode(dev_mode mode);

/* Msg lifecycle */
mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num);
mqtt_msg *mqtt_msg_free(mqtt_msg *msg);
/* Send ack/Receive ack */
int mqtt_send_ack(mqtt_deamon_handler *handler, char master_num);
void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num);
/* Retransmit */
int mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num);

/* Write to data channel */
int mqtt_write_d(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);
int mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

/* Write encrypted data */
int mqtt_tls_write(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

/* Wake up the deamon */
void mqtt_deamon_awake(mqtt_deamon_handler *handler, int *err);

/* Start deamon */
int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      uart_config_t *uart_conf,
                      char *server,
                      char *port,
                      // const char *client_id,
                      char *username,
                      char *password,
                      char **chain,
                      void (*socket_resp_handler)(int *err));

/* Kill deamon */
int mqtt_deamon_stop(mqtt_deamon_handler *handler);