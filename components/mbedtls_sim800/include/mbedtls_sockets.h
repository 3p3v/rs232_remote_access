// #if !defined(__MBEDTLS_SOCKET_TEMPLATE_H__)
// #define __MBEDTLS_SOCKET_TEMPLATE_H__

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SIM.h>

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

#define CONFIG_MBEDTLS_SSL_PROTO_TLS1_3


// #if !defined(MBEDTLS_NET_POLL_READ)
// /* compat for older mbedtls */
// #define MBEDTLS_NET_POLL_READ 1
// #define MBEDTLS_NET_POLL_WRITE 1

// int mbedtls_net_poll(mbedtls_net_context *ctx, uint32_t rw, uint32_t timeout)
// {
//     /* XXX this is not ideal but good enough for an example */
//     usleep(300);
//     return 1;
// }
// #endif


typedef struct mbedtls_context {
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt ca_crt;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
} mbedtls_context;


/*
    A template for opening a non-blocking mbed TLS connection.
*/

int socket_set_handler( mbedtls_context *ctx, void (*resp_handler)(int *) );

int open_nb_socket(mbedtls_context *ctx,
                    char *hostname,
                    char *port,
                    unsigned char **ca_chain);

void close_nb_socket(mbedtls_context *ctx);

// #endif
