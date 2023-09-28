#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

int sim_deamon_start(TaskHandle_t *hander);
int sim_deamon_stop(TaskHandle_t *hander);
TaskHandle_t *sim_deamon_get_task();
TaskHandle_t sim_deamon_create_task();
TaskHandle_t sim_deamon_delete_task();