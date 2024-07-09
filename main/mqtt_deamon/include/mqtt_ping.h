#pragma once

struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

void start_ping_timer(mqtt_deamon_handler *handler);

void reload_ping_timer(mqtt_deamon_handler *handler);

void stop_ping_timer(mqtt_deamon_handler *handler);