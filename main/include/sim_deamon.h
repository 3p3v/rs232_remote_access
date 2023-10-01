#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* SIM800-specyfic libraries */
#include <SIM.h>

typedef struct sim_deamon_handler
{
    TaskHandle_t handler;
    SIM_intf sim;
    // void (*resp_handler)(const unsigned char *);
    void (*error_handler)(void *handler, const char *module, int type, int err);
    // void (*hard_error_handler)(TaskHandle_t *);;
} sim_deamon_handler;

int sim_deamon_start(sim_deamon_handler *handler);

int sim_deamon_stop(sim_deamon_handler *handler);

// TaskHandle_t *sim_deamon_get_task();

TaskHandle_t sim_deamon_create_task(sim_deamon_handler *handler);

TaskHandle_t sim_deamon_delete_task(sim_deamon_handler *handler);