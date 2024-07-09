#pragma once

struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

int mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

int mqtt_write_d(mqtt_deamon_handler *handler, char num, unsigned char *data, size_t len, unsigned char *buf);

int mqtt_subscribe(mqtt_deamon_handler *handler, char channel);