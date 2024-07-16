#include <mbedtls_sockets.h>
#include <net_sockets_sim800.h>
#include <SIM_TCPIP.h>
#include <esp_err.h>
#include <esp_log.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/esp_debug.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include <psa/crypto.h>
#endif
#include <esp_crt_bundle.h>

#define TAG "SIM_encrypt"

int socket_write(mbedtls_context *ctx_, unsigned char *buf, int len)
{
    mbedtls_ssl_context *ctx = &ctx_->ssl_ctx;

    int sent = 0;
    while (sent < len)
    {
        int rv = mbedtls_ssl_write(ctx, buf, len);
        if (rv < 0)
        {
            if (rv == MBEDTLS_ERR_SSL_WANT_READ ||
                rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
            )
            {
                continue;
            }
            break;
        }

        sent += rv;
    }

    return sent;
}

unsigned int socket_buf_check(mbedtls_context *ctx)
{
    return AT_socket_buf_check(&ctx->net_ctx);
}

int socket_read(mbedtls_context *ctx_, unsigned char *buf, int len)
{
    int rv;
    mbedtls_ssl_context *ctx = &ctx_->ssl_ctx;

    do
    {
        rv = mbedtls_ssl_read(ctx, buf, len);
        if (rv > 0)
        {
            /* Enough data, return */
            return len;
        }
        else if (rv == 0)
        {
            /* Socket closed */
            return -1;
        }
        else if (rv < 0)
        {
            if (rv == -1)
            {
                /* No data in buffer */
                rv = 0;
                return rv;
            }
            else if (rv == MBEDTLS_ERR_SSL_WANT_READ)
            {
                rv = 0;
                continue;
            }
            else if (rv == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
            {
                /* Ignore it */
                rv = 0;
                continue;
            }
            else if (rv == MBEDTLS_ERR_SSL_WANT_WRITE
#if defined(MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS)
                     || rv == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS
#endif
#if defined(MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS)
                     || rv == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS
#endif
            )
            {
                /* should call mbedtls_ssl_read later again */
                return 0;
            }

            /* Other error? */
            rv = -1;
            break;
        }
    } while (1);

    return rv;
}

int socket_set_handler(mbedtls_context *ctx, void (*resp_handler)(int *))
{
    return SIM_listenTCP_setHandler(ctx->net_ctx.ctx->sim, ctx->net_ctx.fd, resp_handler);
}

void socket_close_nb(mbedtls_context *ctx)
{
    return mbedtls_net_close_(&ctx->net_ctx);
}

int socket_open_nb(mbedtls_context *ctx,
                   char *hostname,
                   char *port,
                   unsigned char *(*get_cert)(unsigned char),
                   unsigned char chain_size,
                   AT_socket_context *socket_ctx)
{
    char buf[512];
    int ret, flags;

    AT_socket *net = &ctx->net_ctx;
    mbedtls_ssl_context *ssl = &ctx->ssl_ctx;
    mbedtls_ssl_config *ssl_conf = &ctx->ssl_conf;
    mbedtls_x509_crt *ca_crt = &ctx->ca_crt;
    mbedtls_entropy_context *entropy = &ctx->entropy;
    mbedtls_ctr_drbg_context *ctr_drbg = &ctx->ctr_drbg;

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS)
    {
        ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int)status);
        return status;
    }
#endif

    mbedtls_ssl_init(ssl);
    mbedtls_x509_crt_init(ca_crt);
    mbedtls_ctr_drbg_init(ctr_drbg);
    ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(ssl_conf);

    mbedtls_entropy_init(entropy);
    if ((ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func, entropy,
                                     NULL, 0)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }

    ESP_LOGI(TAG, "Attaching the certificate bundle...");
    ret = esp_crt_bundle_attach(ssl_conf);

    if (ret < 0)
    {
        ESP_LOGE(TAG, "esp_crt_bundle_attach returned -0x%x", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");
    /* Hostname set here should match CN in server certificate */
    if ((ret = mbedtls_ssl_set_hostname(ssl, hostname)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");
    if ((ret = mbedtls_ssl_config_defaults(ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    ESP_LOGI(TAG, "mbedtls_ssl_conf_authmode...");
    mbedtls_ssl_conf_authmode(ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);

    for (unsigned char i = chain_size; i > 0; i--)
    {
        /* Lad cert */
        unsigned char *ca_file = get_cert(i - 1);

        if (ca_file == NULL)
        {
            ESP_LOGE(TAG, "DID NOT FIND CERTYFICATE, RETURNING");
            ret = -1;
            goto exit;
        }

        ret = mbedtls_x509_crt_parse(ca_crt, ca_file, strlen((const char *)ca_file) + 1);

        /* Free cert */
        free(ca_file);

        if (ret != 0)
        {
            ESP_LOGE(TAG, "PARSING FAILED, RETURNING");
            goto exit;
        }
    }

    ESP_LOGI(TAG, "mbedtls_ssl_conf_ca_chain...");
    mbedtls_ssl_conf_ca_chain(ssl_conf, ca_crt, NULL);
    mbedtls_ssl_conf_rng(ssl_conf, mbedtls_ctr_drbg_random, ctr_drbg);

#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(ssl_conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_ssl_conf_min_tls_version(ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
    mbedtls_ssl_conf_max_tls_version(ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
#endif
    if ((ret = mbedtls_ssl_setup(ssl, ssl_conf)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
        goto exit;
    }

    ESP_LOGI(TAG, "mbedtls_net_init...");
    mbedtls_net_init_(net, socket_ctx);
    ESP_LOGI(TAG, "mbedtls_net_connect...");
    if ((ret = mbedtls_net_connect_(net, hostname, port)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
        goto exit;
    }

    ESP_LOGI(TAG, "Connected.");
    mbedtls_ssl_set_bio(ssl, net, mbedtls_net_send, mbedtls_net_recv, NULL);

    ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");
    while ((ret = mbedtls_ssl_handshake(ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
            goto exit;
        }
    }

    ESP_LOGI(TAG, "Verifying peer X.509 certificate...");
    if ((flags = mbedtls_ssl_get_verify_result(ssl)) != 0)
    {
        ESP_LOGW(TAG, "Failed to verify peer certificate!");
        bzero(buf, sizeof(buf));
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
    }
    else
    {
        ESP_LOGI(TAG, "Certificate verified.");
    }

exit:
    return ret;
}
