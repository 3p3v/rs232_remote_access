#include <info_ch.h>
#include <mqtt_deamon.h>
#include <uart_deamon.h>
#include <cmd_defs.h>
#include <mqtt_ch_rw.h>

int handle_info_channel(mqtt_deamon_handler *handler, unsigned char *data, size_t len_)
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
        if ((endl = cmdchr(next_cmd_ptr, rem_len, ENDL_C)) == NULL)
        {
            /* Ill-formed command */
            next_cmd_ptr = (char *)data + len_;
            len = add_cmd_none(&channel_data, len, ILL_FORMED_CMD);

            goto exit;
        }
        else if ((arg_ptr = cmdcmp(GET_INFO, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Return current settings */
            uart_cmd_conf conf = handler->uart_get_conf(handler->uart_handler);

            /* Baud rate */
            len = add_cmd_uint(&channel_data, len, GET_BAUD_RATE, conf.baud_rate);

            /* Parity */
            len = add_cmd(&channel_data, len, GET_PARITY, conf.parity);

            /* Char size */
            len = add_cmd_uint(&channel_data, len, GET_CHAR_SIZE, conf.char_size);

            /* Stop bits */
            len = add_cmd(&channel_data, len, GET_STOP_BITS, conf.stop_bits);

            
        }
        else if ((arg_ptr = cmdcmp(MASTER_KEEP_ALIVE, next_cmd_ptr, (endl - next_cmd_ptr))) != NULL)
        {
            /* Keep alive request */
            len = add_cmd_none(&channel_data, len, SLAVE_KEEP_ALIVE);
        }
        else
        {
            /* Unknown cmd */
            len = add_cmd_none(&channel_data, len, UNKNOWN_CMD);
        }

        next_cmd_ptr = endl + 1;

        exit:
        rem_len = len_ - (next_cmd_ptr - (char *)data);

        if (rem_len == 0)
        {
            mqtt_daemon_code err = mqtt_daemon_ok;

            /* Send */
            if (len > 0)
            {
                /* Send */
                err = mqtt_write_i(handler, (unsigned char *)channel_data, len);
            }

            free(channel_data);

            return err;
        }
    }
}