#include <set_ch.h>
#include <esp_log.h>

#define TAG "MQTT_S_CH"

int handle_set_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len_)
{
    char *arg_ptr = NULL;
    char *next_cmd_ptr = (char *)data;
    size_t rem_len = len_;
    char *endl = NULL;

    /* Data to send container */
    char *channel_data = NULL;
    size_t len = 0;

    /* Set property */
    MQTTProperties properties = MQTTProperties_initializer;

    while (1)
    {
        /* Find endl */
        if ((endl = cmdchr(next_cmd_ptr, rem_len, ENDL_C)) == NULL)
        {
            /* Ill-formed command */
            next_cmd_ptr = (char *)data + len_;
            len = add_cmd_none(&channel_data, len, ILL_FORMED_CMD);

            goto exit;
        }
        else if ((arg_ptr = cmdcmp_arg(SET_BAUD_RATE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Argument in cstring to int */
            char *arg_str = (char *) malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int new_buad_rate = atoi(arg_str);
            free(arg_str);

            if (new_buad_rate > BAUD_RATE_MAX || new_buad_rate < BAUD_RATE_MIN)
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
        else if ((arg_ptr = cmdcmp_arg(SET_PARITY, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {   
            char arg_str[2] = {0};
            *arg_str = *arg_ptr;
            
            switch (*arg_ptr)
            {
            case PARITY_EVEN_C:
            {
                handler->uart_conf->parity = UART_PARITY_EVEN;
                break;
            }
            case PARITY_ODD_C:
            {
                handler->uart_conf->parity = UART_PARITY_ODD;
                break;
            }
            case PARITY_NONE_C:
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
        else if ((arg_ptr = cmdcmp_arg(SET_CHAR_SIZE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Argument in cstring to uint */
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
        else if ((arg_ptr = cmdcmp_arg(SET_STOP_BITS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {   
            char arg_str[4] = {0};
            *arg_str = *arg_ptr;
            
            switch (*arg_ptr)
            {
            case STOP_BITS_ONE_C:
            {
                if (*(arg_ptr + 1) == STOP_BITS_ONEPOINTFIVE_C1)
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
            case STOP_BITS_TWO_C:
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
        else if ((arg_ptr = cmdcmp(SET_RTS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            gpio_set_level(MQTT_RTS_PIN, 1);
        }
        else if ((arg_ptr = cmdcmp(RESET_RTS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            gpio_set_level(MQTT_RTS_PIN, 0);
        }
        else if ((arg_ptr = cmdcmp(MASTER_HI, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            handler->m_clean_session = true;
            len = add_cmd_none(&channel_data, len, SLAVE_HI);
        }
        else if ((arg_ptr = cmdcmp(INVALID_PACKET_NUM, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Argument in cstring to int */
            char *arg_str = (char *) malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int slave_num = atoi(arg_str);
            free(arg_str);

            if (mqtt_retransmit(handler, (char)slave_num) <= 0)
            {
                len = add_cmd_uint(&channel_data, len, NO_PACKET_NUMBER, slave_num);
            }
        }
        else if ((arg_ptr = cmdcmp(PACKET_ACK, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Argument in cstring to int */
            char *arg_str = (char *) malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int slave_num = atoi(arg_str);
            free(arg_str);

            mqtt_rec_ack(handler, slave_num);
        }
        else if ((arg_ptr = cmdcmp(MODE_DCE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            set_mode(dce);
        }
        else if ((arg_ptr = cmdcmp(MODE_DTE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            set_mode(dte);
        }
        else
        {
            /* Unknown cmd */
            ESP_LOGE(TAG, "UNKNOWN_CMD");
            len = add_cmd_none(&channel_data, len, UNKNOWN_CMD);
        }

        end:
        /* Set ptr to next command */
        next_cmd_ptr = endl + 1;

        exit:
        /*  */
        rem_len = len_ - (next_cmd_ptr - (char *)data);

        if (rem_len == 0)
        {
            /* Send */
            mqtt_write_i(handler, (unsigned char *)channel_data, len);
            free(channel_data);

            return 0;
        }
    }
}