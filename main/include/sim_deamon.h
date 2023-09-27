#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
/* SIM800-specyfic libraries */
#include <SIM.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
/* Main deamon */
#include <error_handler.h>

extern SIM_intf *sim;

int sim_deamon_start(TaskHandle_t *hander)
{
    /* Initialize the SIM card module */
    SIM_error err = SIM_ok;
    SIM_cmd cmd;
    sim = malloc(sizeof(SIM_intf));
    LL_SIM_def(sim);
    sim->buf = malloc(sizeof(unsigned char) * 12000);
    sim->buf_len = 12000;
    LL_SIM_init(sim);

    /* Start the SIM deamon */
    *handler = sim_deamon_create_task();
    if (!(*handler))
    {
        return SIM_err;
    }

    /* Enable network connection and multiple TCP streams */
    if ((err = SIM_run(sim, SIM_execATE0(&cmd))) ||
        (err = SIM_run(sim, SIM_readCGATT(&cmd))) ||
        (err = SIM_run(sim, SIM_writeCIPMUX(&cmd, 1))) ||
        (err = SIM_run(sim, SIM_writeCSTT(&cmd, "internet", NULL, NULL))) ||
        (err = SIM_run(sim, SIM_execCIICR(&cmd))) ||
        (err = SIM_run(sim, SIM_execCIFSR(&cmd))))
    {
        echo_error("SIM", err);
    }

    return (int)err;
}

int sim_deamon_stop(TaskHandle_t *hander)
{
    /* Stop the SIM deamon */
    *handler = sim_deamon_delete_task();

    if (!(*handler))
    {
        /* Deamon stopped, clean up */
        free(sim->buf);
        free(sim);
        return SIM_ok;
    }
    else
    {
        /* Error occured */
        return SIM_err;
    }
    
}

TaskHandle_t *sim_deamon_get_task()
{
    static TaskHandle_t handle = NULL;
    return &handle;
}

TaskHandle_t sim_deamon_create_task()
{
    TaskHandle_t *handle = sim_deamon_get_task();
    if (!(*handle))
    {
       xTaskCreate(SIM_receiveHandler, "SIM_listener", 5000, (void *)sim, 4, handle);
    }
    return *handle;
}

TaskHandle_t sim_deamon_delete_task()
{
    TaskHandle_t *handle = sim_deamon_get_task();
    if (*handle)
    {
        vTaskDelete(*handle);
    }
    return *handle;
}