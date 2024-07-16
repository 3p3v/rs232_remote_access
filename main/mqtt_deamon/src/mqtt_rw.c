#include <mqtt_rw.h>
#include <mqtt_deamon.h>
/* MbedTLS & ESP32 specyfic libraries */

#include <esp_log.h>
#include <mqtt_ping.h>
#include <mbedtls_sockets.h>

int mqtt_tls_write_no_ping(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        xSemaphoreTake(handler->mbed_mutex_handle, portMAX_DELAY);
        ESP_LOGI(TAG, "TRYING SSL WRITE, socket: %i", handler->ctx.net_ctx.fd);

        int sent = socket_write(&handler->ctx, buf, len);
        if (sent == 0)
        {
            ESP_LOGI(TAG, "SSL WRITE, returned: 0");
            xSemaphoreGive(handler->mbed_mutex_handle);
            return (int)mqtt_daemon_werr;
        }

        ESP_LOGI(TAG, "SSL WRITE DONE, wrote: %i", len);
        xSemaphoreGive(handler->mbed_mutex_handle);

        return sent;
    }
    else
    {
        if (handler == NULL)
            ESP_LOGI(TAG, "MQTT STRUCTURE NULL");
        else
            ESP_LOGI(TAG, "MQTT STRUCTURE NOT INITIALIZED");

        return (int)mqtt_daemon_structure_not_inited;
    }
}

int mqtt_tls_write(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    int sent;

    if ((sent = mqtt_tls_write_no_ping(handler, buf, len)) > 0)
    {
        /* Reset timer */
        reload_ping_timer(handler);
    }

    return sent;
}

int mqtt_tls_read(mqtt_deamon_handler *handler, unsigned char *buf, int len)
{
    if ((len = socket_read(&handler->ctx, buf, len)) > 0)
    {
        return len;
    }
    else
    {
        return mqtt_daemon_rerr;
    }
}