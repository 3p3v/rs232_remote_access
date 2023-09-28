#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


/* Definitions */
#define MAIN_SECS_TILL_REBOOT 10

void echo_error(const char *tag, int err);
void return_error(const char *tag, int err);
QueueHandle_t *error_get_queue();
void error_delete_queue();