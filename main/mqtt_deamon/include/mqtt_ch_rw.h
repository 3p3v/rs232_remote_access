#pragma once

#include <mqtt_deamon.h>

mqtt_daemon_code mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

mqtt_daemon_code mqtt_write_d(mqtt_deamon_handler *handler, char num, unsigned char *data, size_t len, unsigned char *buf);

mqtt_daemon_code mqtt_subscribe(mqtt_deamon_handler *handler, char channel);

mqtt_daemon_code mqtt_connect(mqtt_deamon_handler *handler, char *username, char *password);

/* Msg lifecycle */
// mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num);
// mqtt_msg *mqtt_msg_free(mqtt_msg *msg);

/* Send ack/Receive ack */
mqtt_daemon_code mqtt_send_ack(mqtt_deamon_handler *handler, char master_num);
void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num);

/* Retransmit */
mqtt_daemon_code mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num);

mqtt_daemon_code mqtt_transmit(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);