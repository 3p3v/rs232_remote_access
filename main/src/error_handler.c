#include <error_handler.h>

#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>

/* Definitions */
#define MAIN_SECS_TILL_REBOOT 10

void echo_error(ext_error *err)
{
    
    /* Short info */
    if (err->type == ext_type_non_fatal)
        printf("Module %s encountered non-fatal error: %i.\r\n", err->module, err->err);
    else
        printf("Module %s encountered fatal error: %i.\r\n", err->module, err->err);
    // /* Description if known */
    // switch (err)
    // {
    // default:
    // {
    //     break;
    // }
    // }
}

void reboot()
{
    /* Reboot device in x */
    char i = MAIN_SECS_TILL_REBOOT;
    for (;;)
    {
        printf("Reboot in %i s.\r\n", i);
        if (i != 0)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            break;
        }
        i--;
    }

    printf("Rebooting...\r\n");
    // abort();
    while(1);
}

void ext_error_send(void *handler, const char *module, int type, int err)
{
    ext_error ext_err = {.handler = *((TaskHandle_t *)handler),
                         .module = malloc(sizeof(char) * (strlen(module) + 1)),
                         .type = (ext_type)type,
                         .err = err};
    memcpy(ext_err.module, module, strlen(module) + 1);

    QueueHandle_t *queue = error_get_queue();
    xQueueSend(*queue, (void *)&ext_err, portMAX_DELAY);
}

// void send_error(TaskHandle_t *handler)
// {
//     QueueHandle_t *queue = error_get_queue();
//     xQueueSend(*queue, handler, sizeof(TaskHandle_t *));
// }

void ext_mqtt_send_error(ext_error *err)
{

}

void return_error(const char *tag, int err)
{
    /* Short info */
    printf("Module %s encountered unhandlable error: %i.\r\n", tag, err);
    /* Description if known */
    switch (err)
    {
    default:
    {
        break;
    }
    }
}

QueueHandle_t *error_create_queue()
{
    QueueHandle_t *queue = error_get_queue();
    *queue = xQueueCreate(1, sizeof(ext_error));
    return queue;
}

QueueHandle_t *error_get_queue()
{
    static QueueHandle_t queue;
    return &queue;
}

void error_delete_queue()
{
    QueueHandle_t *queue = error_get_queue();
    vQueueDelete(*queue);
}