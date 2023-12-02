#include <mbedtls_sockets.h>
#include <SIM_TCPIP.h>
#include <esp_err.h>
#include <esp_log.h>

#define TAG "SIM_encrypt"

extern SIM_intf *sim;

int socket_set_handler( mbedtls_context *ctx, void (*resp_handler)(int *) )
{
    return SIM_listenTCP_setHandler(sim, ctx->net_ctx.fd, resp_handler);
}

void close_nb_socket(mbedtls_context *ctx)
{
    mbedtls_net_close(&ctx->net_ctx);
}

int open_nb_socket(mbedtls_context *ctx,
                    char *hostname,
                    char *port,
                    unsigned char **ca_chain)
{
    char buf[512];
    int ret, flags;

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
        ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int) status);
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
    mbedtls_net_init(net);
    ESP_LOGI(TAG, "mbedtls_net_connect...");
    if ((ret = mbedtls_net_connect(net, hostname,
                                   port, MBEDTLS_NET_PROTO_TCP)) != 0)
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

