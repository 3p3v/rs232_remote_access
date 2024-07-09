#pragma once
#include <mbedtls_sockets.h>

struct mqtt_deamon_handler;
typedef struct mqtt_deamon_handler mqtt_deamon_handler;

/* Write encrypted data */
int mqtt_tls_write(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

int mqtt_tls_write_no_ping(mqtt_deamon_handler *handler, unsigned char *buf, size_t len);

int mqtt_tls_read(mbedtls_ssl_context *ctx, unsigned char *buf, int len);