#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* SIM800-specyfic libraries */
#include <SIM.h>

#define SIM_DAEMON_TASK_STACK_SIZE 4000
#define SIM_DAEMON_TASK_NAME "SIM_DAEMON"
#define SIM_DAEMON_TASK_PRIORITY 2

#define LINUX_TS_2011 1293840000
#define SIM_DAEMON_BUF_LEN 12000

#define SIM_DAEMON_AT_WAIT 100
#define SIM_DAEMON_AT_CYCLES 15

#define SIM_DAEMON_CREG_WAIT 1000
#define SIM_DAEMON_CREG_CYCLES 15

#define SIM_DAEMON_CGATT_WAIT 1000
#define SIM_DAEMON_CGATT_CYCLES 50

typedef struct sim_deamon_handler
{
    TaskHandle_t handler;
    SIM_intf sim;
    void (*error_handler)(void *handler, const char *module, int type, int err);
} sim_deamon_handler;

void sim_deamon(void *v_handler);

int sim_deamon_start(
    sim_deamon_handler *handler,
    char *apn,
    char *username,
    char *password);

int sim_deamon_stop(sim_deamon_handler *handler);

TaskHandle_t sim_deamon_create_task(sim_deamon_handler *handler);

TaskHandle_t sim_deamon_delete_task(sim_deamon_handler *handler);