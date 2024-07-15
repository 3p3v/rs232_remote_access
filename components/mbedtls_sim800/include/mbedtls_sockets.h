#pragma once

// #define CONFIG_MBEDTLS_SSL_PROTO_TLS1_3

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <AT_sockets.h>
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include "psa/crypto.h"
#endif
#include "esp_crt_bundle.h"

typedef struct mbedtls_context
{
    AT_socket net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt ca_crt;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
} mbedtls_context;

void socket_reinit(mbedtls_context *ctx, AT_socket_context socket_ctx);

int socket_read(mbedtls_context *ctx_, unsigned char *buf, int len);

int socket_set_handler(mbedtls_context *ctx, void (*resp_handler)(int *));

int socket_open_nb(
    mbedtls_context *ctx,
    char *hostname,
    char *port,
    unsigned char *(*get_cert)(unsigned char),
    unsigned char chain_size,
    AT_socket_context *socket_ctx);

void socket_close_nb(mbedtls_context *ctx);
