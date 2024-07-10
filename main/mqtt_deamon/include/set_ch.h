#pragma once

#include <mqtt_deamon.h>
#include <uart_deamon.h>
#include <cmd_defs.h>

mqtt_daemon_code handle_set_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len);