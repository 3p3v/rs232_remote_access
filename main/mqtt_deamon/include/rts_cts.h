#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/* RTS */
#define MQTT_RTS_PIN GPIO_NUM_21
/* CTS */
#define MQTT_CTS_PIN GPIO_NUM_19

typedef enum
{
    dte = 0,
    dce
} dev_mode;
 
void rts_cts_set_mode(dev_mode mode, QueueHandle_t *queue);