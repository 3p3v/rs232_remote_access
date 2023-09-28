#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define MAIN_MQTT_REC_BUF_SIZE 1024
#define MAIN_MQTT_SEND_BUF_SIZE 1024

QueueHandle_t *mqtt_deamon_get_queue();
void mqtt_deamon_delete_queue();
void mqtt_deamon(void *);
void mqtt_deamon_awake(int *err);
TaskHandle_t *mqtt_deamon_get_task();
TaskHandle_t mqtt_deamon_create_task();
TaskHandle_t mqtt_deamon_delete_task();
int mqtt_deamon_start(TaskHandle_t *hander, void (*publish_callback)(void **, struct mqtt_response_publish *));
int mqtt_deamon_stop(TaskHandle_t *hander);