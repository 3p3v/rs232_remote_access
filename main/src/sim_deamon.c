#include <sim_deamon.h>

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

int sim_deamon_start(sim_deamon_handler *handler)
{
    /* Initialize the SIM card module */
    SIM_error err = SIM_ok;
    SIM_cmd cmd;
    // sim = malloc(sizeof(SIM_intf));
    LL_SIM_def(&handler->sim);
    sim->buf = malloc(sizeof(unsigned char) * 12000);
    sim->buf_len = 12000;
    LL_SIM_init(&handler->sim);

    /* Start the SIM deamon */
    sim_deamon_create_task(handler);
    if (!(handler->handler))
    {
        return SIM_err;
    }

    /* Enable network connection and multiple TCP streams */ // TODO enable later
    // if ((err = SIM_run(&handler->sim, SIM_execATE0(&cmd))) ||
    //     (err = SIM_run(&handler->sim, SIM_readCGATT(&cmd))) ||
    //     (err = SIM_run(&handler->sim, SIM_writeCIPMUX(&cmd, 1))) ||
    //     (err = SIM_run(&handler->sim, SIM_writeCSTT(&cmd, "internet", NULL, NULL))) ||
    //     (err = SIM_run(&handler->sim, SIM_execCIICR(&cmd))) ||
    //     (err = SIM_run(&handler->sim, SIM_execCIFSR(&cmd))))
    // {
    //     handler->error_handler("SIM", err);
    // }

    err = SIM_run(&handler->sim, SIM_execATE0(&cmd));
    ESP_LOGI("DEAMON", "0");
    err = SIM_run(&handler->sim, SIM_readCGATT(&cmd));
    ESP_LOGI("DEAMON", "1");
    err = SIM_run(&handler->sim, SIM_writeCIPMUX(&cmd, 1));
    ESP_LOGI("DEAMON", "2");
    err = SIM_run(&handler->sim, SIM_writeCSTT(&cmd, "internet", NULL, NULL));
    ESP_LOGI("DEAMON", "3");
    err = SIM_run(&handler->sim, SIM_execCIICR(&cmd));
    ESP_LOGI("DEAMON", "4");
    err = SIM_run(&handler->sim, SIM_execCIFSR(&cmd));
    ESP_LOGI("DEAMON", "5");
    err = SIM_ok;

    return (int)err;
}

int sim_deamon_stop(sim_deamon_handler *handler)
{
    /* Stop the SIM deamon */
    sim_deamon_delete_task(handler);

    if (!(handler))
    {
        /* Deamon stopped, clean up */
        free(handler->sim.buf);
        return SIM_ok;
    }
    else
    {
        /* Error occured */
        return SIM_err;
    }
}

// TaskHandle_t *sim_deamon_get_task()
// {
//     static TaskHandle_t handle = NULL;
//     return &handle;
// }

TaskHandle_t sim_deamon_create_task(sim_deamon_handler *handler)
{
    if (!(handler->handler))
    {
        xTaskCreate(SIM_receiveHandler, "SIM_listener", 4000, (void *)(&handler->sim), 4, &handler->handler);
    }
    return handler->handler;
}

TaskHandle_t sim_deamon_delete_task(sim_deamon_handler *handler)
{
    if (handler->handler)
    {
        vTaskDelete(handler->handler);
    }
    return handler->handler;
}