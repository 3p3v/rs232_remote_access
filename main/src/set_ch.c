#include <set_ch.h>

int handle_set_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len_)
{
    char *arg_ptr = NULL;
    char *next_cmd_ptr = data;
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
            next_cmd_ptr = data + len_;
            len = add_cmd_none(&channel_data, len, ILL_FORMED_CMD);
        }
        else if ((arg_ptr = cmdcmp(SET_BAUD_RATE, next_cmd_ptr, rem_len)) != NULL)
        {
            /* Argument in cstring to u=int */
            char *arg_str = (char *) malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int new_buad_rate = atoi(arg_str);
            free(arg_str);

            if (new_buad_rate > BAUD_RATE_MAX || new_buad_rate > BAUD_RATE_MIN)
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_BAUD_RATE); 
                goto end;
            }

            /* Set UART */
            handler->uart_conf->baud_rate = new_buad_rate;
            uart_set_baudrate(UART_DEAMON_DEF_UART_NUM, handler->uart_conf->baud_rate);
            uart_flush(UART_DEAMON_DEF_UART_NUM);
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            
            /* Add baud rate set cmd */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, handler->uart_conf->baud_rate);            
        }
        else if ((arg_ptr = cmdcmp(SET_PARITY, next_cmd_ptr, rem_len)) != NULL)
        {   
            char arg_str[2] = {0};
            *arg_str = *arg_ptr;
            
            switch (*arg_ptr)
            {
            case PARITY_EVEN[0]:
            {
                handler->uart_conf->parity = UART_PARITY_EVEN;
                break;
            }
            case PARITY_ODD[0]:
            {
                handler->uart_conf->parity = UART_PARITY_ODD;
                break;
            }
            case PARITY_NONE[0]:
            {
                handler->uart_conf->parity = UART_PARITY_DISABLE;
                break;
            }
            default:
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_PARITY); 
                goto end;
                break;
            }
            }
            
            /* Change parity */
            uart_set_parity(UART_DEAMON_DEF_UART_NUM, handler->uart_conf->parity);
            uart_flush(UART_DEAMON_DEF_UART_NUM);
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            
            /* Add parity set cmd */
            len = add_cmd(&channel_data, len, GET_PARITY, arg_str);
        }
        else if ((arg_ptr = cmdcmp(SET_CHAR_SIZE, next_cmd_ptr, rem_len)) != NULL)
        {
            /* Argument in cstring to u=int */
            char *arg_str = (char *) malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int new_char_size = atoi(arg_str);
            free(arg_str);

            if (new_char_size > CHAR_SIZE_MAX || new_char_size > CHAR_SIZE_MIN)
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_CHAR_SIZE); 
                goto end;
            }

            /* Set char size */
            handler->uart_conf->data_bits = new_char_size;
            uart_set_word_length(UART_DEAMON_DEF_UART_NUM, handler->uart_conf->data_bits);
            uart_flush(UART_DEAMON_DEF_UART_NUM);
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            
            /* Add baud rate set cmd */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, handler->uart_conf->baud_rate);            
        }
        else if ((arg_ptr = cmdcmp(SET_STOP_BITS, next_cmd_ptr, rem_len)) != NULL)
        {   
            char arg_str[4] = {0};
            *arg_str = *arg_ptr;
            
            switch (*arg_ptr)
            {
            case STOP_BITS_ONE[0]:
            {
                if (*(arg_ptr + 1) == STOP_BITS_ONEPOINTFIVE[1])
                {
                    /* STOP_BITS_ONEPOINTFIVE */
                    handler->uart_conf->stop_bits = UART_STOP_BITS_1_5;
                    strcpy(arg_str, STOP_BITS_ONEPOINTFIVE);
                }
                else if (*(arg_ptr + 1) == ENDL_C)
                {
                    /* STOP_BITS_ONE */
                    handler->uart_conf->stop_bits = UART_STOP_BITS_1;
                    *arg_str = STOP_BITS_ONE[0];
                }
                else
                {
                    /* Invalid argument */
                    len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_STOP_BITS); 
                    goto end;
                    break;
                }
                break;
            }
            case STOP_BITS_TWO[0]:
            {
                handler->uart_conf->stop_bits = UART_STOP_BITS_2;
                *arg_str = STOP_BITS_TWO[0];
                break;
            }
            default:
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_STOP_BITS); 
                goto end;
                break;
            }
            }
            
            /* Change parity */
            uart_set_stop_bits(UART_DEAMON_DEF_UART_NUM, handler->uart_conf->stop_bits);
            uart_flush(UART_DEAMON_DEF_UART_NUM);
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            
            /* Add parity set cmd */
            len = add_cmd(&channel_data, len, GET_STOP_BITS, arg_str);
        }
        else
        {
            /* Unknown cmd */
            len = add_cmd_none(&channel_data, len, UNKNOWN_CMD);
        }

        end:
        next_cmd_ptr = endl + 1;
        rem_len = len_ - (next_cmd_ptr - data);

        if (rem_len == 0)
        {
            /* Send */
            unsigned char *buf = (unsigned char *) malloc(sizeof(unsigned char) * (len + 100));
            int write_len = MQTTV5Serialize_publish(buf, 1024, 0, QOS, 0, 0, topicString, &properties, channel_data, len);
            mqtt_tls_write(buf, write_len);

            free(channel_name);
            free(channel_data);
            free(buf);

            return 0;
        }
    }
}