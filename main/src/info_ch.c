#include <info_ch.h>

int handle_info_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len_)
{
    char *arg_ptr = NULL;
    char *next_cmd_ptr = (char *)data;
    size_t rem_len = len_;
    char *endl = NULL;

    /* Data to send container */
    char *channel_data = NULL;
    size_t len = 0;

    /* Create name */
    MQTTString topicString = MQTTString_initializer;
    char *channel_name = get_channel_name(handler->username, INFO_CH_C);
    topicString.cstring = channel_name;

    /* Set property */
    MQTTProperties properties = MQTTProperties_initializer;

    while (1)
    {
        if ((endl = cmdchr(next_cmd_ptr, rem_len, ENDL_C)) == NULL)
        {
            /* Ill-formed command */
            next_cmd_ptr = (char *)data + len_;
            len = add_cmd_none(&channel_data, len, ILL_FORMED_CMD);
        }
        else if ((arg_ptr = cmdcmp(GET_INFO, next_cmd_ptr, rem_len)) != NULL)
        {
            /* Return current settings */

            /* Baud rate */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, handler->uart_conf->baud_rate);

            /* Parity */
            char *parity_arg = NULL;
            switch (handler->uart_conf->parity)
            {
            case UART_PARITY_DISABLE:
            {
                parity_arg = PARITY_NONE;
                break;
            }
            case UART_PARITY_EVEN:
            {
                parity_arg = PARITY_EVEN;
                break;
            }
            case UART_PARITY_ODD:
            {
                parity_arg = PARITY_ODD;
                break;
            }
            }
            len = add_cmd(&channel_data, len, GET_PARITY, parity_arg);

            /* Char size */
            len = add_cmd_uint(&channel_data, len, GET_CHAR_SIZE, handler->uart_conf->data_bits);

            /* Stop bits */
            char *stop_bits_arg = NULL;
            switch (handler->uart_conf->stop_bits)
            {
            case UART_STOP_BITS_1:
            {
                stop_bits_arg = STOP_BITS_ONE;
                break;
            }
            case UART_STOP_BITS_1_5:
            {
                stop_bits_arg = STOP_BITS_ONEPOINTFIVE;
                break;
            }
            case UART_STOP_BITS_2:
            {
                stop_bits_arg = STOP_BITS_TWO;
                break;
            }
            case UART_STOP_BITS_MAX:
            {
                len = add_cmd(&channel_data, len, GET_STOP_BITS, INVALID_OPTION);
                break;
            }
            }
            len = add_cmd(&channel_data, len, GET_STOP_BITS, stop_bits_arg);

            
        }
        else if ((arg_ptr = cmdcmp(MASTER_KEEP_ALIVE, next_cmd_ptr, rem_len)) != NULL)
        {
            /* Keep alive request */
            len = add_cmd_none(&channel_data, len, SLAVE_KEEP_ALIVE);
        }
        else if ((arg_ptr = cmdcmp(MASTER_KEEP_ALIVE, next_cmd_ptr, rem_len)) != NULL)
        {
            /* Hi */
            len = add_cmd_none(&channel_data, len, SLAVE_HI);
        }
        else
        {
            /* Unknown cmd */
            len = add_cmd_none(&channel_data, len, UNKNOWN_CMD);
        }

        next_cmd_ptr = endl + 1;
        rem_len = len_ - (next_cmd_ptr - (char *)data);

        if (rem_len == 0)
        {
            /* Send */
            unsigned char *buf = (unsigned char *) malloc(sizeof(unsigned char) * (len + 100));
            int write_len = MQTTV5Serialize_publish(buf, 1024, 0, QOS, 0, 0, topicString, &properties, (unsigned char *)channel_data, len);
            mqtt_tls_write(handler, buf, write_len);

            free(channel_name);
            free(channel_data);
            free(buf);

            return 0;
        }
    }
}