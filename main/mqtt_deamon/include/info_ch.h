#pragma once

struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

int handle_info_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len);