#include <set_ch.h>
#include <esp_log.h>
#include <mqtt_ch_rw.h>
#include <rts_cts.h>
#include <error_handler.h>

#define TAG "MQTT_S_CH"

mqtt_daemon_code handle_set_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len_)
{
    char *arg_ptr = NULL;
    char *next_cmd_ptr = (char *)data;
    size_t rem_len = len_;
    char *endl = NULL;

    /* Data to send container */
    char *channel_data = NULL;
    size_t len = 0;

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
            char *arg_str = (char *)malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int new_buad_rate = atoi(arg_str);
            free(arg_str);

            if (new_buad_rate > BAUD_RATE_MAX || new_buad_rate < BAUD_RATE_MIN)
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_BAUD_RATE);
                continue;
            }

            /* Set UART */
            handler->uart_change_conf(handler->uart_handler, uart_sett_baud_rate, &new_buad_rate);

            /* Add baud rate set cmd */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, new_buad_rate);
        }
        else if ((arg_ptr = cmdcmp_arg(SET_PARITY, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            char arg_str[2];
            arg_str[0] = arg_ptr[0];
            arg_str[1] = '\0';

            switch (*arg_ptr)
            {
            case PARITY_EVEN_C:
            case PARITY_ODD_C:
            case PARITY_NONE_C:
            {
                handler->uart_change_conf(handler->uart_handler, uart_sett_parity, arg_ptr);
                break;
            }
            default:
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_PARITY);
                continue;
                break;
            }
            }

            /* Add parity set cmd */
            len = add_cmd(&channel_data, len, GET_PARITY, arg_str);
        }
        else if ((arg_ptr = cmdcmp_arg(SET_CHAR_SIZE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Argument in cstring to uint */
            char *arg_str = (char *)malloc(sizeof(char) * (endl - arg_ptr + 1));
            memcpy(arg_str, arg_ptr, endl - arg_ptr);
            arg_str[endl - arg_ptr] = '\0';
            int new_char_size = atoi(arg_str);
            free(arg_str);

            if (new_char_size > CHAR_SIZE_MAX || new_char_size > CHAR_SIZE_MIN)
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_CHAR_SIZE);
                continue;
            }

            /* Set char size */
            handler->uart_change_conf(handler->uart_handler, uart_sett_char_size, &new_char_size);

            /* Add baud rate set cmd */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, new_char_size);
        }
        else if ((arg_ptr = cmdcmp_arg(SET_STOP_BITS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            char arg_str[2];
            arg_str[0] = arg_ptr[0];
            arg_str[1] = '\0';

            switch (*arg_ptr)
            {
            case STOP_BITS_ONE_C:
            case STOP_BITS_ONEPOINTFIVE_C:
            case STOP_BITS_TWO_C:
            {
                handler->uart_change_conf(handler->uart_handler, uart_sett_char_size, arg_str);

                break;
            }
            default:
            {
                /* Invalid argument */
                len = add_cmd(&channel_data, len, INVALID_ARGUMENT, SET_STOP_BITS);
                continue;
                break;
            }
            }

            /* Add parity set cmd */
            len = add_cmd(&channel_data, len, GET_STOP_BITS, arg_str);
        }
        else if ((arg_ptr = cmdcmp(SET_RTS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            rts_cts_set(handler);
        }
        else if ((arg_ptr = cmdcmp(RESET_RTS, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            rts_cts_set(handler);
        }
        else if ((arg_ptr = cmdcmp(MASTER_HI, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            len = add_cmd_none(&channel_data, len, SLAVE_HI);
        }
        else if ((arg_ptr = cmdcmp(INVALID_PACKET_NUM, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            char temp[2] = {'\0'};
            temp[0] = *arg_ptr;

            int err;
            if ((err = mqtt_retransmit(handler, *arg_ptr)) == mqtt_daemon_packet_not_found)
            {
                len = add_cmd(&channel_data, len, NO_PACKET_NUMBER, *temp);
            }
            else if (err == mqtt_daemon_werr)
            {
                handler->error_handler(&handler->handler, "MQTT", ext_type_fatal, -1);
            }
        }
        else if ((arg_ptr = cmdcmp(PACKET_ACK, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            mqtt_rec_ack(handler, *arg_ptr);
        }
        else if ((arg_ptr = cmdcmp(NO_PACKET_NUMBER, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            len = add_cmd_none(&channel_data, len, SLAVE_HI);
        }
        else if ((arg_ptr = cmdcmp(MODE_DCE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            rts_cts_set_mode(handler, dce);
        }
        else if ((arg_ptr = cmdcmp(MODE_DTE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            rts_cts_set_mode(handler, dte);
        }
        else
        {
            /* Unknown cmd */
            ESP_LOGE(TAG, "UNKNOWN_CMD");
            len = add_cmd_none(&channel_data, len, UNKNOWN_CMD);
        }

        /* Set ptr to next command */
        next_cmd_ptr = endl + 1;

    exit:
        /*  */
        rem_len = len_ - (next_cmd_ptr - (char *)data);

        if (rem_len == 0)
        {
            mqtt_daemon_code err = mqtt_daemon_ok;

            if (len > 0)
            {
                /* Send */
                err = mqtt_write_i(handler, (unsigned char *)channel_data, len);
            }

            free(channel_data);

            return 0;
        }
    }
}