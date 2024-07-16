#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

/* RTS */
#define MQTT_RTS_PIN GPIO_NUM_21
/* CTS */
#define MQTT_CTS_PIN GPIO_NUM_19

struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

typedef enum
{
    dte = 0,
    dce
} dev_mode;
 
void rts_cts_set_mode(mqtt_deamon_handler *handler, dev_mode mode);

void rts_cts_set(mqtt_deamon_handler *handler);

void rts_cts_reset(mqtt_deamon_handler *handler);