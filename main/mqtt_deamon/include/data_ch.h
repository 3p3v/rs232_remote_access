#pragma once

#include <mqtt_deamon.h>

mqtt_daemon_code handle_data_channel(mqtt_deamon_handler *handler, unsigned char *payload, size_t payload_len, char packet_num, bool num_found);