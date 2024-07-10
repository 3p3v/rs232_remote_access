#include <mqtt_ch_rw.h>
#include <mqtt_deamon.h>
#include <mqtt_rw.h>
#include <cmd_defs.h>
/* MQTT lib */
#include <MQTTV5Packet.h>
#include <esp_log.h>

static void clean_properties(MQTTProperties *properties)
{
    properties->count = 0;
    properties->length = 0;
}

mqtt_daemon_code mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        mqtt_daemon_code err = mqtt_daemon_ok;
        
        /* Create name */
        char *channel_name = get_channel_name(handler->username, INFO_CH_C);
        MQTTString channel_name_s = MQTTString_initializer;
        channel_name_s.cstring = channel_name;

        xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

        /* Set property */
        MQTTProperties ack_properties = MQTTProperties_initializer;

        if((len = MQTTV5Serialize_publish(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, QOS, 0, 0, channel_name_s, &ack_properties, buf, len)) <= 0)
        {
            err = mqtt_daemon_serialize_error;
        }
        else if (mqtt_tls_write(handler, handler->sendbuf, len) <= 0)
        {
            err = mqtt_daemon_werr;
        }

        xSemaphoreGive(handler->send_buf_mutex_handle);
        free(channel_name);

        return err;
    }
    else
    {
        return mqtt_daemon_structure_not_inited;
    }
}

mqtt_daemon_code mqtt_write_d(mqtt_deamon_handler *handler, char num, unsigned char *data, size_t len, unsigned char *buf)
{
    if (len > MAX_SLAVE_DATA_LEN)
    {
        /* Data too long */
        ESP_LOGE(TAG, "DATA TO SEND TOO LONG");
        return mqtt_daemon_data_too_long;
    }

    if (handler != NULL && handler->initialized == true)
    {
        mqtt_daemon_code err = mqtt_daemon_ok;
        
        /* Get name */
        char *channel_name = get_channel_name(handler->username, DATA_CH_C);
        MQTTString topic_str = MQTTString_initializer;
        topic_str.cstring = channel_name;

        /* Set property */
        MQTTProperty ack_properties_array[1];
        MQTTProperties ack_properties = MQTTProperties_initializer;
        ack_properties.array = ack_properties_array;
        ack_properties.max_count = 1;

        MQTTProperty packet_num = {
            .identifier = MQTTPROPERTY_CODE_USER_PROPERTY,
            .value.string_pair.key.data = PACKET_NUM_KEY,
            .value.string_pair.key.len = strlen(PACKET_NUM_KEY),
            .value.string_pair.val.data = (char *)&num,
            .value.string_pair.val.len = PACKET_NUM_MAX};
        MQTTProperties_add(&ack_properties, &packet_num);

        /* Send ACK and set length */
        if((len = MQTTV5Serialize_publish(buf, MAX_SLAVE_PACKET_LEN, 0, QOS, 0, 0, topic_str, &ack_properties, data, len)) <= 0)
        {
            len = mqtt_daemon_serialize_error;

            goto EXIT;
        }
        else if (mqtt_tls_write(handler, buf, len) <= 0)
        {
            err = mqtt_daemon_werr;

            goto EXIT;
        }

        mqtt_num_up(&handler->slave_num);
        handler->s_not_ack++;
        ESP_LOGE(TAG, "NOT ACKED: %u", handler->s_not_ack);

        if (handler->s_not_ack > MAX_NOT_ACK)
        {
            ESP_LOGE(TAG, "PACKET OVERFLOW MSG");
            char *msg = NULL;
            int len_ = add_cmd_none(&msg, 0, PACKET_OVERFLOW);
            err = mqtt_write_i(handler, (unsigned char *)msg, len_);
            free(msg);

            if (err < 0)
            {
                err = mqtt_daemon_werr;
            }
        }

        EXIT:
        free(channel_name);

        return err;
    }
    else
    {
        return mqtt_daemon_structure_not_inited;
    }
}

mqtt_daemon_code mqtt_subscribe(mqtt_deamon_handler *handler, char channel)
{
    mqtt_daemon_code err = mqtt_daemon_ok;
    
    /* Subscribe to channels */
    char *channel_name = get_channel_name(handler->username, channel);
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = channel_name;
    MQTTProperties sub_properties = MQTTProperties_initializer;
    MQTTSubscribe_options sub_options = {.noLocal = 1, .retainAsPublished = 0, .retainHandling = 0};

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

    /* Send subscribe */
    int len;
    if ((len = MQTTV5Serialize_subscribe(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, handler->msg_id++, &sub_properties, 1, &topicString, &handler->qos, &sub_options)) <= 0)
    {
        err = mqtt_daemon_serialize_error;
    }
    else if (mqtt_tls_write(handler, handler->sendbuf, len) <= 0)
    {
        err = mqtt_daemon_werr;
    }
    
    xSemaphoreGive(handler->send_buf_mutex_handle);
    /* Deallocate name */
    free(channel_name);

    return err;
}

mqtt_daemon_code mqtt_connect(mqtt_deamon_handler *handler, char *username, char *password)
{
    mqtt_daemon_code err = mqtt_daemon_ok;
    
    /* Will channel */
    char *channel_name = get_channel_name(handler->username, INFO_CH_C);
    MQTTString will_topic = MQTTString_initializer;
    will_topic.cstring = channel_name;

    /* Will message */
    char *will_str = DEV_DISCONNECT;
    MQTTString will_msg = MQTTString_initializer;
    will_msg.cstring = will_str;

    /* Will properties */
    MQTTProperties will_props = MQTTProperties_initializer;

    /* Will */
    MQTTV5Packet_willOptions will = MQTTV5Packet_willOptions_initializer;
    will.topicName = will_topic;
    will.message = will_msg;
    will.qos = QOS;
    will.properties = &will_props;

    /* Connection parameters */
    MQTTV5Packet_connectData connect_data = MQTTV5Packet_connectData_initializer;
    connect_data.username.cstring = username;
    connect_data.password.cstring = password;
    connect_data.MQTTVersion = 5;
    connect_data.keepAliveInterval = 90;
    connect_data.willFlag = 1;
    connect_data.will = will;

    /* Connection properties, none needed */
    MQTTProperties conn_properties = MQTTProperties_initializer;

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

    int len;
    if ((len = MQTTV5Serialize_connect(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, &connect_data, &conn_properties)) <= 0)
    {
        err = mqtt_daemon_serialize_error;
    }
    else if (mqtt_tls_write_no_ping(handler, handler->sendbuf, len) <= 0)
    {
        err = mqtt_daemon_werr;
    }
    
    xSemaphoreGive(handler->send_buf_mutex_handle);
    free(channel_name);

    return err;
}

static mqtt_msg *mqtt_msg_intializer(mqtt_msg *msg, char num)
{
    msg->ack = false;
    msg->num = num;

    return msg;
}

static mqtt_msg *mqtt_msg_free(mqtt_msg *msg)
{
    msg->ack = true;

    return msg;
}

mqtt_daemon_code mqtt_send_ack(mqtt_deamon_handler *handler, char master_num)
{
    /* Zero notack */
    handler->m_not_ack = 0;
    /* Send ack */
    char *data;
    size_t len = add_cmd(&data, 0, PACKET_ACK, &master_num);
    mqtt_daemon_code err = mqtt_write_i(handler, (unsigned char *)data, len);
    free(data);
    return err;
}

void mqtt_rec_ack(mqtt_deamon_handler *handler, char slave_num)
{
    if (mqtt_if_bigger(slave_num, handler->slave_num))
    {
        /* Should never happen */
        return;
    }
    
    /* Decrement unack msgs */
    handler->s_not_ack = handler->slave_num - slave_num - 1;

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

mqtt_daemon_code mqtt_transmit(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);

    /* Find msg to delete */
    mqtt_msg *msg_ptr = NULL;
    for (unsigned char i = 0; i < MAX_SAVED; i++)
    {
        if (handler->send_msgs[i].ack == true)
        {
            /* Picking the first acked packet's place */
            msg_ptr = &handler->send_msgs[i];
            mqtt_msg_intializer(msg_ptr, handler->slave_num);
            break;
        }
    }

    /* No space found? */
    if (msg_ptr == NULL)
    {
        /* No space for data */
        ESP_LOGE(TAG, "NOT ENOUGH SPACE FOR NEW PACKET, CHOOSING THE OLDEST ONE");
        /* Find oldest packet */
        msg_ptr = &handler->send_msgs[0];
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
    }

    mqtt_daemon_code err = mqtt_write_d(handler, msg_ptr->num, buf, len, msg_ptr->data);

    xSemaphoreGive(handler->send_msgs_mutex_handle);

    return err;
}

mqtt_daemon_code mqtt_retransmit(mqtt_deamon_handler *handler, char slave_num)
{
    if (handler != NULL && handler->initialized == true)
    {
        xSemaphoreTake(handler->send_msgs_mutex_handle, portMAX_DELAY);

        /* Do while id we are searching for is lesser or equal newest message id */
        while (mqtt_if_smaller_or_equal(slave_num, handler->slave_num))
        {
            ESP_LOGI(TAG, "FINDING PACKET: %c", slave_num);

            mqtt_msg *msg = NULL;
            for (unsigned char i = 0; i < MAX_SAVED; i++)
            {
                if (handler->send_msgs[i].num == slave_num && handler->send_msgs[i].ack == false)
                {
                    ESP_LOGI(TAG, "PACKET: %c FOUND", slave_num);

                    msg = &handler->send_msgs[i];

                    break;
                }
            }

            if (msg == NULL)
            {
                ESP_LOGE(TAG, "PACKET: %c NOT FOUND", slave_num);

                xSemaphoreGive(handler->send_msgs_mutex_handle);

                return mqtt_daemon_packet_not_found;
            }

            /* Send first message */
            if ( mqtt_tls_write(handler, msg->data, msg->len) < 0)
            {
                /* ERROR */
                xSemaphoreGive(handler->send_msgs_mutex_handle);

                return mqtt_daemon_werr;
            }
            else
            {
                /* next number */
                mqtt_num_up_(&slave_num);
            }
        }
    }

    ESP_LOGI(TAG, "STRUCTURE NOT INITIALIZED");

    return mqtt_daemon_structure_not_inited;
}
