#include <mqtt_rw.h>
#include <mqtt_deamon.h>
/* MbedTLS & ESP32 specyfic libraries */
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/esp_debug.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include <psa/crypto.h>
#endif
#include <esp_crt_bundle.h>

int mqtt_tls_write_no_ping(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        xSemaphoreTake(handler->mbed_mutex_handle, portMAX_DELAY);
        ESP_LOGI(TAG, "TRYING SSL WRITE, socket: %i", handler->ctx.net_ctx.fd);

        size_t sent = 0;
        while (sent < len)
        {
            int rv = mbedtls_ssl_write(&handler->ctx.ssl_ctx, buf, len);
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
                    /* should call mbedtls_ssl_write later again */
                    continue;
                }
                break;
            }
            /*
             * Note: rv can be 0 here eg. when mbedtls just flushed
             * the previous incomplete record.
             *
             * Note: we never send an empty TLS record.
             */
            sent += (size_t)rv;
        }
        if (sent == 0)
        {
            ESP_LOGI(TAG, "SSL WRITE, returned: 0");
            xSemaphoreGive(handler->mbed_mutex_handle);
            return -1;
        }
        /* Write to broker */

        ESP_LOGI(TAG, "SSL WRITE DONE, wrote: %i", len);
        xSemaphoreGive(handler->mbed_mutex_handle);
        /* Return */
        return sent;
    }
    else
    {
        if (handler == NULL)
            ESP_LOGI(TAG, "MQTT STRUCTURE NULL");
        else
            ESP_LOGI(TAG, "MQTT STRUCTURE NOT INITIALIZED");

        return -1;
    }
}

int mqtt_tls_write(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    int sent;

    if ((sent = mqtt_tls_write_no_ping(handler, buf, len)) > 0)
    {
        /* Reset timer */
        reload_ping_timer(handler);

        return sent;
    }
    else
    {
        return -1;
    }
}

int mqtt_tls_read(mbedtls_ssl_context *ctx, unsigned char *buf, int len)
{
    int rv;

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
            /*
             * Note: mbedtls_ssl_read returns 0 when the underlying
             * transport was closed without CloseNotify.
             *
             * Raise an error to trigger a reconnect.
             */
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
                /* We were waiting for application data but got
                 * a NewSessionTicket instead. */
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
            /* Note: MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY is handled here. */
            // printf("NOT SERIOUS SOCKET ERROR?");
            rv = -1;
            break;
        }
    } while (1);

    return rv;
}