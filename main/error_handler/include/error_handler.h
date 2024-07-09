#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

/* Definitions */
#define MAIN_SECS_TILL_REBOOT 10

#define ERROR_HANDLER_TASK_NAME "ERROR_DEAMON"

typedef enum ext_type
{
    ext_type_non_fatal = 0,
    ext_type_fatal
} ext_type;

typedef struct ext_error
{
    TaskHandle_t handler;
    // char *module;
    ext_type type;
    int err;
} ext_error;

void echo_error(const char *module, ext_error *err);
void ext_mqtt_send_error(const char *module, ext_error *err);
void return_error(const char *tag, int err);
void send_error(TaskHandle_t *handler);
QueueHandle_t *error_create_queue();
QueueHandle_t *error_get_queue();
void error_delete_queue();
void ext_error_send(void *handler, const char *module, int type, int err);