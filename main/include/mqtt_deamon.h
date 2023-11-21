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

#define MAIN_MQTT_REC_BUF_SIZE 1024
#define MAIN_MQTT_SEND_BUF_SIZE 1024

#define PING_REQUEST 1000

#define SEC_IN_MIN 60
#define PING_TIMER_RES 1000000
#define QOS 0
#define PROPERTIES_SIZE 5
#define USERNAME_LEN strlen("XX:XX:XX:XX:XX:XX")

typedef struct mqtt_deamon_handler
{
    TaskHandle_t handler;
    gptimer_handle_t ping_timer;
    SemaphoreHandle_t send_buf_mutex_handle;
    unsigned char sendbuf[MAIN_MQTT_SEND_BUF_SIZE];
    SemaphoreHandle_t mbed_mutex_handle;
    MQTTV5Transport trp;
    uart_config_t *uart_conf;
    bool initialized;
    /* Number of next msg to send */
    unsigned int slave_num;
    /* Number of next message from master */
    unsigned int master_num;// = 0;
    /* Master asked for a new session */
    bool m_clean_session;// = false;
    int keep_alive_int;
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

/* Write to data channel */
int mqtt_write_d(unsigned char *buf, size_t len);

/* Write encrypted data */
int mqtt_tls_write(unsigned char *buf, size_t len);

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