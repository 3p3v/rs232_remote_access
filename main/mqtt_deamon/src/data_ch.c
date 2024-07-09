#include <data_ch.h>
#include <mqtt_deamon.h>
#include <cmd_defs.h>
#include <mqtt_ch_rw.h>

mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num)
{
    msg->ack = false;
    msg->num = num;

    return msg;
}

mqtt_msg *mqtt_msg_free(mqtt_msg *msg)
{
    msg->ack = true;

    return msg;
}

int mqtt_send_ack(mqtt_deamon_handler *handler, char master_num)
{
    /* Zero notack */
    handler->m_not_ack = 0;
    /* Send ack */
    char *data;
    size_t len = add_cmd(&data, 0, PACKET_ACK, &master_num);
    len = mqtt_write_i(handler, (unsigned char *)data, len);
    free(data);
    return len;
}

void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num)
{
    /* Decrement unack msgs */
    char temp = handler->slave_num - slave_num - 1;

    if (temp < 0)
    {
        temp += MAX_MSG_NUM - MIN_MSG_NUM;
    }

    handler->s_not_ack = temp;

    /* Delete msgs */
    char max = slave_num;
    char min;

    if ((slave_num % MIN_MSG_NUM) < (MAX_NOT_ACK))
    {
        min = MAX_MSG_NUM - (slave_num % MIN_MSG_NUM);
    }
    else
    {
        min = max - MAX_NOT_ACK;
    }

    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        if (max < min)
        {
            if (handler->send_msgs[i].num <= max || handler->send_msgs[i].num >= min)
            {
                handler->send_msgs[i].ack = true;
            }
        }
        else
        {
            if (handler->send_msgs[i].num <= max && handler->send_msgs[i].num >= min)
            {
                handler->send_msgs[i].ack = true;
            }
        }
    }
}


int mqtt_transmit(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);
    
    /* Find msg to delete */
    unsigned char *msg_data_ptr = NULL;
    size_t *len_ptr = NULL;
    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        if (handler->send_msgs[i].ack == true)
        {
            /* Picking the first acked packet's place */
            mqtt_msg_intializer(&handler->send_msgs[i], handler->slave_num);
            msg_data_ptr = handler->send_msgs[i].data;
            len_ptr = &handler->send_msgs[i].len;
            break;
        }
    }

    /* No space found? */
    if (msg_data_ptr == NULL)
    {
        /* No space for data */
        ESP_LOGE(TAG, "NOT ENOUGH SPACE FOR NEW PACKET, CHOOSING THE OLDEST ONE");
        /* Find oldest packet */
        mqtt_msg *msg_ptr = &handler->send_msgs[0];
        for (unsigned char i = 0; i < MAX_SAVED; i++)
        {
            if ((handler->slave_num % MIN_MSG_NUM) < (MAX_NOT_ACK))
            {
                if (msg_ptr->num > handler->slave_num && msg_ptr->num > handler->send_msgs[i].num)
                {
                    msg_ptr = &handler->send_msgs[i];
                }
            }
            else
            {
                if (msg_ptr->num > handler->send_msgs[i].num)
                {
                    msg_ptr = &handler->send_msgs[i];
                }
            }
        }

        /* Set oldest packet as a place for a new one */
        mqtt_msg_intializer(msg_ptr, handler->slave_num);
        msg_data_ptr = msg_ptr->data;
        len_ptr = &msg_ptr->len;
    }

    *len_ptr = mqtt_write_d(handler, msg_ptr->num, buf, len, msg_data_ptr);

    xSemaphoreGive(handler->send_msgs_mutex_handle);

    return *len_ptr
}


int mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num)
{
    if (handler != NULL && handler->initialized == true)
    {
        int len = -2;

        xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);

        while(1)
        {
            ESP_LOGI(TAG, "FINDING PACKET: %c", slave_num);

            mqtt_msg *msg = NULL;
            for (unsigned char i = 0; i < MAX_SAVED; i++)
            {
                ESP_LOGI(TAG, "FOUND PACKET: %c", handler->send_msgs[i].num);

                if (handler->send_msgs[i].num == slave_num && handler->send_msgs[i].ack == false)
                {
                    ESP_LOGI(TAG, "PACKET: %c FOUND", slave_num);

                    msg = &handler->send_msgs[i];

                    break;
                }
            }

            /* If first was found len if >= 0, else < 0 */
            if (msg == NULL)
            {
                ESP_LOGE(TAG, "PACKET: %c NOT FOUND", slave_num);

                xSemaphoreGive(handler->send_msgs_mutex_handle);
                return len;
            }

            /* Send first message */
            if ((len = mqtt_tls_write(handler, msg->data, msg->len)) <= 0)
            {
                /* ERROR */
                xSemaphoreGive(handler->send_msgs_mutex_handle);

                return len;
            }
            else
            {
                /* next number */
                mqtt_num_up_(&slave_num);
            }
        }
    }
    else
    {
        ESP_LOGI(TAG, "STRUCTURE NOT INITIALIZED");

        return -1;
    }
}