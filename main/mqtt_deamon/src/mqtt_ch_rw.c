#include <mqtt_ch_rw.h>
#include <mqtt_deamon.h>
#include <mqtt_rw.h>
#include <cmd_defs.h>
/* MQTT lib */
#include <MQTTV5Packet.h>

static void clean_properties(MQTTProperties *properties)
{
    properties->count = 0;
    properties->length = 0;
}

int mqtt_write_i(mqtt_deamon_handler *handler, unsigned char *buf, size_t len)
{
    if (handler != NULL && handler->initialized == true)
    {
        /* Create name */
        char *channel_name = get_channel_name(handler->username, INFO_CH_C);
        MQTTString channel_name_s = MQTTString_initializer;
        channel_name_s.cstring = channel_name;

        xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

        /* Set property */
        MQTTProperties ack_properties = MQTTProperties_initializer;

        /* Send ACK */
        int ser_len = MQTTV5Serialize_publish(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, QOS, 0, 0, channel_name_s, &ack_properties, buf, len);
        mqtt_tls_write(handler, handler->sendbuf, ser_len);

        xSemaphoreGive(handler->send_buf_mutex_handle);
        free(channel_name);
        return ser_len;
    }
    else
    {
        return -1;
    }
}

int mqtt_write_d(mqtt_deamon_handler *handler, char num, unsigned char *data, size_t len, unsigned char *buf)
{
    if (len > MAX_SLAVE_DATA_LEN)
    {
        /* Data too long */
        ESP_LOGE(TAG, "DATA TO SEND TOO LONG");
        return -2;
    }

    if (handler != NULL && handler->initialized == true)
    {
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
        if(MQTTV5Serialize_publish(buf, MAX_SLAVE_PACKET_LEN, 0, QOS, 0, 0, topic_str, &ack_properties, data, len) < len)
        {
            return -1;
        }
        else if ((len = mqtt_tls_write(handler, buf, len)) <= len)
        {
            return -1;
        }

        mqtt_num_up(&handler->slave_num);
        handler->s_not_ack++;
        ESP_LOGE(TAG, "NOT ACKED: %u", handler->s_not_ack);
        free(channel_name);

        if (handler->s_not_ack > MAX_NOT_ACK)
        {
            ESP_LOGE(TAG, "PACKET OVERFLOW MSG");
            char *msg = NULL;
            int len_ = add_cmd_none(&msg, 0, PACKET_OVERFLOW);
            mqtt_write_i(handler, (unsigned char *)msg, len_);
            free(msg);
        }

        return len;
    }
    else
    {
        return -1;
    }
}

int mqtt_subscribe(mqtt_deamon_handler *handler, char channel)
{
    /* Subscribe to channels */
    char *channel_name = get_channel_name(handler->username, channel);
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = channel_name;
    MQTTProperties sub_properties = MQTTProperties_initializer;
    MQTTSubscribe_options sub_options = {.noLocal = 1, .retainAsPublished = 0, .retainHandling = 0};

    xSemaphoreTake(handler->send_buf_mutex_handle, portMAX_DELAY);

    /* Send subscribe */
    int len = MQTTV5Serialize_subscribe(handler->sendbuf, MAIN_MQTT_SEND_BUF_SIZE, 0, handler->msg_id++, &sub_properties, 1, &topicString, &handler->qos, &sub_options);
    len = mqtt_tls_write(handler, handler->sendbuf, len);

    xSemaphoreGive(handler->send_buf_mutex_handle);

    /* Deallocate name */
    free(channel_name);

    return len;
}

