#include <data_ch.h>
#include <mqtt_deamon.h>
#include <cmd_defs.h>
#include <mqtt_ch_rw.h>
#include <esp_log.h>

mqtt_daemon_code handle_data_channel(mqtt_deamon_handler *handler, unsigned char *payload, size_t payload_len, char packet_num, bool num_found)
{
    mqtt_daemon_code err = mqtt_daemon_ok;
    
    if (num_found == false)
    {
        /* Send no packet number error, throw away this packet */
        char *data_to_send = NULL;
        ESP_LOGE(TAG, "NO_PACKET_NUMBER");
        size_t len = add_cmd_none(&data_to_send, 0, NO_PACKET_NUMBER);

        err = mqtt_write_i(handler, (unsigned char *)data_to_send, len);

        free(data_to_send);
    }
    else if (packet_num != handler->master_num)
    {
        /* Sent invalid packet number error, ask for valid one, throw away this packet */
        char *data_to_send = NULL;
        ESP_LOGE(TAG, "INVALID_PACKET_NUM");
        size_t len = add_cmd(&data_to_send, 0, INVALID_PACKET_NUM, &handler->master_num);

        err = mqtt_write_i(handler, (unsigned char *)data_to_send, len);

        free(data_to_send);
    }
    else
    {
        /* Increment unack */
        handler->m_not_ack++;

        /* Send data to UART */
        handler->publish_callb(payload, payload_len);
        /* Send ack if exceded ACK_AFTER */
        if (handler->m_not_ack > ACK_AFTER)
        {
            err = mqtt_send_ack(handler, handler->master_num);
            
            handler->m_not_ack = 0;
        }
        /* Set next packet number */
        mqtt_num_up(&handler->master_num);
    }

    return err;
}
