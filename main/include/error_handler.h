#pragma once

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

void echo_error(const char *tag, int err)
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
    abort();
}

QueueHandle_t *error_get_queue()
{
    static QueueHandle_t queue = xQueueCreate(1, sizeof(QueueHandle_t));
    return &queue;
}

void error_delete_queue()
{
    QueueHandle_t *queue = error_get_queue();
    vQueueDelete(*queue);
}