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
#include <sys/time.h>
/* SIM800-specyfic libraries */
#include <SIM.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
/* Main deamon */
#include <error_handler.h>
/**/
#include <string.h>

#define TAG "SIM"

extern SIM_intf *sim;

static struct timeval convert_time(char *t_str)
{
    struct tm tm;
    strptime(t_str, "%y/%m/%d,%H:%M:%S", &tm);
    time_t t = mktime(&tm);
    struct timeval tv;
    tv.tv_sec = t;
    tv.tv_usec = 0;
    return tv;
}

int sim_deamon_start(sim_deamon_handler *handler)
{
    /* Initialize the SIM card module */
    SIM_error err = SIM_ok;
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
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

    unsigned char cycles = 15;

    start:
    
    /* Restart module */
    ESP_LOGI(TAG, "RESETTING SIM");
    if ((err = SIM_reset(&cmd)) != SIM_ok)
        return SIM_err;
    
    /* Communicate with SIM module */
    ESP_LOGI(TAG, "SENDING AT");
    do
    {
        err = SIM_run(&handler->sim, SIM_execAT(&cmd));
        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while (err != SIM_ok && cycles-- > 0);

    if (err != SIM_ok)
        return err;

    /* Disable echo */
    err = SIM_run(&handler->sim, SIM_execATE0(&cmd));
    
    cycles = 15;

    /* Check if registered to network, it can take some time... */
    ESP_LOGI(TAG, "CHECKING NETWORK REGISTARTION");
    do
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        err = SIM_run(&handler->sim, SIM_readCREG(&cmd));
        if (cmd.resp.params_num > 1 && cmd.resp.params[1].len != 0)
        {
            char *net_reg = cmd.resp.params[1].ptr;
            if (*net_reg == '1' ||
                *net_reg == '5')
                break;
        }
    } while (err != SIM_ok && cycles-- > 0);

    /* Get time from network */
    ESP_LOGI(TAG, "GETTING NETWORK TIME");
    if ((err = SIM_run(&handler->sim, SIM_readCCLK(&cmd))) != SIM_ok ||
        cmd.resp.params_num < 1)
        return SIM_err;

    /* Convert time */
    char t_str[18] = {0};
    memcpy(t_str, cmd.resp.params[0].ptr, cmd.resp.params[0].len);
    struct timeval tv_now = convert_time(t_str);
    /* Check if time ok */
    if (tv_now.tv_sec < LINUX_TS_2011)
    {
        /* Seems that SIM is not synced with network */
        /* Enable time sync */
        ESP_LOGI(TAG, "TIME NOT SYNCED, ENABLING SYNCED...");
        if ((err = SIM_run(&handler->sim, SIM_writeCLTS(&cmd, 1))) != SIM_ok)
            return SIM_err;

        /* Save active configuration */
        ESP_LOGI(TAG, "SAVING CONFIG");
        if ((err = SIM_run(&handler->sim, SIM_execATandW(&cmd))) != SIM_ok)
            return SIM_err;

        /* Reboot SIM and retry */
        goto start;
    }

    /* Set time in case it's needed to check a validity of certyficates */
    settimeofday(&tv_now, NULL);

    cycles = 50;
    
    /* Check if attached to GPRS */
    do
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        err = SIM_run(&handler->sim, SIM_readCGATT(&cmd));
        if (cmd.resp.params_num > 0)
        {
            ESP_LOGI(TAG, "+CGATT: %s" ,cmd.resp.params[0].ptr);
            if (cmd.resp.params[0].ptr[1] == '1')
                break;
        }
        else
        {
            return SIM_err;
        }
    } while (err == SIM_ok && cycles-- > 0);

    if (!(cmd.resp.params_num > 0 && cmd.resp.params[0].ptr[1] == '1'))
        return SIM_err;
    
    err = SIM_run(&handler->sim, SIM_writeCIPMUX(&cmd, 1));
    err = SIM_run(&handler->sim, SIM_writeCSTT(&cmd, "internet", NULL, NULL));
    err = SIM_run(&handler->sim, SIM_execCIICR(&cmd));
    if ((err = SIM_run(&handler->sim, SIM_execCIFSR(&cmd)) != SIM_ok))
        return err;

    err = SIM_ok;

    SIM_cmd_free(&cmd);

    return (int)err;
}

int sim_deamon_stop(sim_deamon_handler *handler)
{
    /* Stop the SIM deamon */
    sim_deamon_delete_task(handler);

    if (!(handler))
    {
        /* Deamon stopped, clean up */
        LL_SIM_deinit(&handler->sim);
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

void sim_deamon(void *v_handler)
{
    int err;
    sim_deamon_handler *handler = (sim_deamon_handler *)v_handler;
    SIM_intf *sim = &handler->sim;

    for (;;)
    {
        /* Receive data */
        err = LL_SIM_receiveRaw(sim);
        if (err < 0)
        {
            switch (err)
            {
            case LL_SIM_HARDWARE_ERR:
            {
                handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
                goto exit;
            }
            default:
            {
                handler->error_handler(&handler->handler, "SIM", ext_type_fatal, err);
                break;
            }
            }
        }

        /* Run execution of a command */
        SIM_exec(sim);
    }

    exit:
    for (;;)
        ;
}

TaskHandle_t sim_deamon_create_task(sim_deamon_handler *handler)
{
    if (!(handler->handler))
    {
        xTaskCreate(sim_deamon, "SIM_listener", 4000, (void *)(handler), 2, &handler->handler);
    }
    return handler->handler;
}

TaskHandle_t sim_deamon_delete_task(sim_deamon_handler *handler)
{
    if (handler->handler)
    {
        vTaskDelete(handler->handler);
        handler->handler = NULL;
    }
    return handler->handler;
}