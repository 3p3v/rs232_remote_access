#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
/* MQTT-C lib */
#include <MQTTClient.h>
/**/
#include <mbedtls_sockets.h>

#define MAIN_MQTT_REC_BUF_SIZE 1024
#define MAIN_MQTT_SEND_BUF_SIZE 1024

#define QOS 0
#define PROPERTIES_SIZE 5
#define USERNAME_LEN strlen("XX:XX:XX:XX:XX:XX")

typedef struct mqtt_deamon_handler
{
    TaskHandle_t handler;
    // TaskHandle_t ping_handle;
    MQTTV5Transport trp;
    QueueHandle_t queue;
    mbedtls_context ctx;
    char *username;
    // void (*publish_callback)(void **, struct mqtt_response_publish *);
    void (*error_handler)(void *, const char *, int , int );
    void (*publish_callb)(unsigned char *, size_t);
    // void (*hard_error_handler)(TaskHandle_t *);
} mqtt_deamon_handler;

// mqtt_deamon_handler *mqtt_deamon_set_handle(void (*error_handler)(const char *, int), void (*hard_error_handler)(const char *, int));

// static mqtt_deamon_handler *mqtt_deamon_get_handle();

// static void mqtt_deamon_delete_queue();

void mqtt_deamon(void * v_mqtt_deamon_handler);

void mqtt_deamon_awake(mqtt_deamon_handler *handler, int *err);
// TaskHandle_t *mqtt_deamon_get_task();

TaskHandle_t mqtt_deamon_create_task(mqtt_deamon_handler *handler);
TaskHandle_t mqtt_deamon_delete_task(mqtt_deamon_handler *handler);
int mqtt_deamon_start(mqtt_deamon_handler *handler,
                      const char *server,
                      const char *port,
                      // const char *client_id,
                      const char *username,
                      const char *password,
                      const unsigned char **chain,
                      void (*socket_resp_handler)(int *err));
int mqtt_deamon_stop(mqtt_deamon_handler *handler);