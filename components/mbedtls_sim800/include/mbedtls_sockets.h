#if !defined(__MBEDTLS_SOCKET_TEMPLATE_H__)
#define __MBEDTLS_SOCKET_TEMPLATE_H__

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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


#if !defined(MBEDTLS_NET_POLL_READ)
/* compat for older mbedtls */
#define MBEDTLS_NET_POLL_READ 1
#define MBEDTLS_NET_POLL_WRITE 1

int mbedtls_net_poll(mbedtls_net_context *ctx, uint32_t rw, uint32_t timeout)
{
    /* XXX this is not ideal but good enough for an example */
    usleep(300);
    return 1;
}
#endif


typedef struct mbedtls_context {
    mbedtls_net_context net_ctx;
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_x509_crt ca_crt;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
} mbedtls_context;
// struct mbedtls_context
// {
//     mbedtls_net_context net_ctx;
//     mbedtls_ssl_context ssl_ctx;
//     mbedtls_ssl_config ssl_conf;
//     mbedtls_x509_crt ca_crt;
//     mbedtls_entropy_context entropy;
//     mbedtls_ctr_drbg_context ctr_drbg;
// };

// void failed(const char *fn, int rv);
// void cert_verify_failed(uint32_t rv);
// void open_nb_socket(struct mbedtls_context *ctx,
//                     const char *hostname,
//                     const char *port,
//                     const char *ca_file);

// void failed(const char *fn, int rv)
// {
//     char buf[100];
//     mbedtls_strerror(rv, buf, sizeof(buf));
//     printf("%s failed with %x (%s)\n", fn, -rv, buf);
//     exit(1);
// }

// void cert_verify_failed(uint32_t rv)
// {
//     char buf[512];
//     mbedtls_x509_crt_verify_info(buf, sizeof(buf), "\t", rv);
//     printf("Certificate verification failed (%0" PRIx32 ")\n%s\n", rv, buf);
//     exit(1);
// }

/*
    A template for opening a non-blocking mbed TLS connection.
*/
int open_nb_socket(mbedtls_context *ctx,
                    const char *hostname,
                    const char *port,
                    const unsigned char **ca_chain)
{
    // int take_break = 1;
    // while(take_break);

    char buf[512];
    int ret, flags, len;

    mbedtls_net_context *net = &ctx->net_ctx;
    mbedtls_ssl_context *ssl = &ctx->ssl_ctx;
    mbedtls_ssl_config *ssl_conf = &ctx->ssl_conf;
    mbedtls_x509_crt *ca_crt = &ctx->ca_crt;
    mbedtls_entropy_context *entropy = &ctx->entropy;
    mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        // ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int) status);
        return status;
    }
#endif

    

    mbedtls_ssl_init(ssl);
    mbedtls_x509_crt_init(ca_crt);
    mbedtls_ctr_drbg_init(ctr_drbg);
    // ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(ssl_conf);

    mbedtls_entropy_init(entropy);
    if ((ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                                     NULL, 0)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }

    // ESP_LOGI(TAG, "Attaching the certificate bundle...");

    ret = esp_crt_bundle_attach(ssl_conf);

    if (ret < 0)
    {
        // ESP_LOGE(TAG, "esp_crt_bundle_attach returned -0x%x", -ret);
        abort();
    }

    // ESP_LOGI(TAG, "Setting hostname for TLS session...");

    /* Hostname set here should match CN in server certificate */
    if ((ret = mbedtls_ssl_set_hostname(ssl, hostname)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    // ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED); // MBEDTLS_SSL_VERIFY_REQUIRED);

    // mbedtls_x509_crt_init(ca_crt);
    int ca_num = 0;
    for (;;)
    {
        const unsigned char *ca_file = ca_chain[ca_num];
        if (!ca_file)
        {
            break;
        }

        ret = mbedtls_x509_crt_parse(ca_crt, ca_file, strlen((const char *)ca_file) + 1);
        if (ret != 0) {
            goto exit;
        }

        ca_num++;
    }

    mbedtls_ssl_conf_ca_chain(ssl_conf, ca_crt, NULL);
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);

    // const char *protocols[] = {"h2", "http/1.1", NULL};
    // mbedtls_ssl_conf_alpn_protocols(&conf, protocols);
#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(ssl_conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif
    // mbedtls_esp_enable_debug_log(ssl_conf, 0);

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_ssl_conf_min_tls_version(ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
    mbedtls_ssl_conf_max_tls_version(ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
#endif
    if ((ret = mbedtls_ssl_setup(ssl, ssl_conf)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
        goto exit;
    }

    // while(1) {
    mbedtls_net_init(net);

    // ESP_LOGI(TAG, "Connecting to %s:%s...", WEB_SERVER, WEB_PORT);

    if ((ret = mbedtls_net_connect(net, hostname,
                                   port, MBEDTLS_NET_PROTO_TCP)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    // // ESP_LOGI(TAG, "Connected.");

    mbedtls_ssl_set_bio(ssl, net, mbedtls_net_send, mbedtls_net_recv, NULL);

    // ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            // ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
            goto exit;
        }
    }

    // ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

    if ((flags = mbedtls_ssl_get_verify_result(ssl)) != 0)
    {
        /* In real life, we probably want to close connection if ret != 0 */
        // ESP_LOGW(TAG, "Failed to verify peer certificate!");
        bzero(buf, sizeof(buf));
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
        // ESP_LOGW(TAG, "verification info: %s", buf);
    }
    else
    {
        // ESP_LOGI(TAG, "Certificate verified.");
    }

    exit:
    return ret;
}

#endif
