#pragma once

struct mqtt_msg;
typedef struct mqtt_msg;

struct mqtt_deamon_handler;
struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

/* Msg lifecycle */
mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num);
mqtt_msg *mqtt_msg_free(mqtt_msg *msg);

/* Send ack/Receive ack */
int mqtt_send_ack(mqtt_deamon_handler *handler, char master_num);
void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num);

/* Retransmit */
int mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num);