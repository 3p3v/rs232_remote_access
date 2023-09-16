// #include "LL_SIMESP.hpp"
// #include "LL_SIMBB.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "string.h"
#include "esp_intr_alloc.h"
#include "soc/soc.h"
#include "hal/uart_ll.h"
#include "esp_log.h"
#include "LL_SIM.h"

static char TAG[] = "LL_SIM";
extern const SIM_err_pair SIM_reservedResps[];

void LL_SIM_def(LL_SIM_intf *sim)
{
    sim->buf = NULL;
    sim->buf_len = 0;
    sim->rec_len = 0;
    sim->unread_num = 0;
    sim->tx = LL_SIM_DEF_UART_TX;
    sim->rx = LL_SIM_DEF_UART_RX;
    sim->baudRate = 19200;
    sim->drt = LL_SIM_DEF_DRT_PIN;
    sim->rst = LL_SIM_DEF_RST_PIN;
    sim->uart = LL_SIM_DEF_UART_NUM;
    sim->uartQueue = NULL; // xQueueCreate(LL_SIM_DEF_QUEUE_SIZE, sizeof(int));
    sim->cmds.cmd = NULL;
    sim->cmds.queue = xQueueCreate(LL_SIM_DEF_CMD_QUEUE_SIZE, sizeof(SIM_error));
    for (int i = 0; i < LL_SIM_DEF_TCP_CMDS_NUM; i++)
        sim->tcp_cmds[i].queue = xQueueCreate(LL_SIM_DEF_CMD_QUEUE_SIZE, sizeof(SIM_error));

    sim->tcp_ret = NULL;

    for (int i = 0; i < SIM_MAX_LINES_ARR_LEN; i++)
    {
        sim->lines[i].ptr = NULL;
        sim->lines[i].len = 0;
    }
    sim->lines_num = 0;

    sim->add_cmd_mutex = xSemaphoreCreateMutex();
    sim->write_mutex = xSemaphoreCreateMutex();
    sim->exec_mutex = xSemaphoreCreateMutex();
}

/*  UART configuration, includes ESP pattern detection (detects every new line) */
LL_SIM_error LL_SIM_init(const LL_SIM_intf *sim)
{
    /* UART */
    uart_config_t uartConf = {
        .baud_rate = sim->baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT};

    // uartQueue =

    /* Basic UART config */ // TODO CHECK!
    ESP_ERROR_CHECK(uart_driver_install(sim->uart, LL_SIM_DEF_TX_BUF_SIZE, LL_SIM_DEF_TX_BUF_SIZE, LL_SIM_DEF_QUEUE_SIZE, &sim->uartQueue, 0));
    ESP_ERROR_CHECK(uart_param_config(sim->uart, &uartConf));
    ESP_ERROR_CHECK(uart_set_pin(sim->uart, sim->tx, sim->rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Pattern detection event config */
    // uart_enable_pattern_det_baud_intr(uart, '\n', 1, LL_SIM_PATTERN_INTERVAL, LL_SIM_MIN_POST_IDLE, LL_SIM_MIN_PRE_IDLE);
    // uart_pattern_queue_reset(uart, LL_SIM_DEF_PATTERN_QUEUE_SIZE);

    /* GPIO */
    gpio_config_t gpioConf = {
        .pin_bit_mask = (uint64_t)((1 << sim->drt) | (1 << sim->rst)),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};

    /* Set DRT and RST pin */
    ESP_ERROR_CHECK(gpio_config(&gpioConf));

    // resetForce();

    // setStatus(LL_SIMUARTInitialised);

    return SIM_ok;
}

LL_SIM_error LL_SIM_deinit(const LL_SIM_intf *sim)
{
    // resetStatus(LL_SIMUARTInitialised);
    if (uart_driver_delete(sim->uart) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}

static void SIM_setMsg(char *buf, unsigned int *rec_len, char *msg_end)
{
    memcpy(buf, msg_end, *rec_len - (msg_end - buf) + 1);
    *rec_len -= msg_end - buf;
}

SIM_error SIM_exec(SIM_intf *sim)
{
    SIM_error err;

    xSemaphoreTake(sim->exec_mutex, portMAX_DELAY);

START:
    // Find all lines in data
    sim->lines_num = SIM_findAllLines(sim->buf, sim->rec_len, sim->lines, SIM_MAX_LINES_ARR_LEN);
    SIM_errMsgEnd_pair err_pair = SIM_retrieveCustomErr(sim->lines, SIM_reservedResps);

    switch (err_pair.err)
    {
    case SIM_noErrCode:
    {
        // for (;;)
        // {
        if (sim->cmds.cmd == NULL)
        {
            err = SIM_noErrCode;
            goto REGION_END;
        }

        err = sim->cmds.cmd->handlers[sim->cmds.cmd->handlers_num - 1](sim->buf, sim->rec_len, &sim->cmds.cmd->resp, (void *)sim);

        // Check execution code
        if (err == SIM_ok)
        {
            // Handler was executed correctly
            SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
            sim->cmds.cmd->handlers_num--;
            if (sim->cmds.cmd->handlers_num == 0)
            {
                // If all handlers were executed then return with ok code and 'delete' cmd from listener
                // sim->cmds.cmd->err = err;
                /* EDIT */
                xQueueSend(sim->cmds.queue, (void *)&err /*&sim->cmds.cmd->resp.err*/, portMAX_DELAY);
                sim->cmds.cmd = NULL;
                /********/
            }

            if (sim->rec_len == 0)
            {
                // no message left
                sim->unread_num = 0;
                goto REGION_END;
            }
            else
            {
                // another message, or part of it left in buffer, let it be processed by another cmd
                goto START;
            }
        }
        else if (err == SIM_noErrCode || err == SIM_noResp)
        {
            // try again untill the cmd isn't removed
            goto REGION_END;
        }
        else
        {
            // ecountered error
            SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message                /* EDIT */
            xQueueSend(sim->cmds.queue, (void *)&err, portMAX_DELAY);
            sim->cmds.cmd = NULL;
            /********/

            if (sim->rec_len == 0)
            {
                // no message left
                sim->unread_num = 0;
                goto REGION_END;
            }
            else
            {
                // another metcpssage, or part of it left in buffer, let it be processed by another cmd
                goto START;
            }
        }
        // }

        break;
    }
    case SIM_receive:
    case SIM_closed:
    {
        char num_str[2] = {};
        unsigned char num;

        if (sim->tcp_ret == NULL)
        {
            if (err_pair.err == SIM_receive)
            {
                num_str[0] = err_pair.ptr_beg[9];
                num = atoi(num_str);
            }
            else
            {
                num_str[0] = err_pair.ptr_beg[0];
                num = atoi(num_str);
            }

            if (sim->tcp_cmds[num].cmd == NULL)
            {
                // Discard maching line
                // TODO
                return SIM_noErrCode;
            }

            sim->tcp_ret = &sim->tcp_cmds[num];
        }

        // for (;;)
        // {
        err = sim->tcp_ret->cmd->handler(sim->buf, sim->rec_len, &sim->tcp_ret->cmd->resp, (void *)sim);

        if (err == SIM_ok)
        {
            // Handler was executed correctly
            SIM_setMsg(sim->buf, &sim->rec_len, sim->tcp_ret->cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
            /* EDIT */
            xQueueSend(sim->tcp_ret->queue, (void *)&sim->tcp_ret->cmd->resp.err, portMAX_DELAY);
            sim->tcp_ret = NULL;
            /********/

            if (sim->rec_len == 0)
            {
                // no message left
                sim->unread_num = 0;
                goto REGION_END;
            }
            else
            {
                // another message, or part of it left in buffer, let it be processed by another cmd
                goto START;
            }
        }
        else if (err == sim_msgDetect)
        {
            // Got some matching output, not enough data though, try again when data arrives
            goto REGION_END;
        }
        else
        {
            // encountered error
            SIM_setMsg(sim->buf, &sim->rec_len, sim->tcp_ret->cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
            /* EDIT */
            xQueueSend(sim->tcp_ret->queue, (void *)&err, portMAX_DELAY);
            sim->tcp_ret = NULL;
            /********/
            // 'delete' cmd from listener
            // sim->tcp_ret->cmd = NULL;

            if (sim->rec_len == 0)
            {
                // no message left
                sim->unread_num = 0;
                goto REGION_END;
            }
            else
            {
                // another message, or part of it left in buffer, let it be processed by another cmd
                goto START;
            }
        }

        // }

        break;
    }
    default:
    {
        // Discard maching line
        // TODO
    }
    }

REGION_END:
    xSemaphoreGive(sim->exec_mutex);
    return err;
}

//     // START: // used to start
//     static int cmd_ret = -1;
//     for (int i = sim->cmds_num - 1; i >= 0; i--)
//     {
//         if (cmd_ret >= 0)
//             i = cmd_ret;

//         switch (sim->cmds.cmd->type)
//         {
//         case SIM_cmd_single_use:
//         {
//             // Execute operations with data received
//             err = sim->cmds.cmd->handlers[sim->cmds.cmd->handlers_num - 1](sim->buf, sim->rec_len, &sim->cmds.cmd->resp, (void *)sim);

//             // Check execution code
//             if (err == SIM_ok)
//             {
//                 // Handler was executed correctly
//                 SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
//                 sim->cmds.cmd->handlers_num--;
//                 if (sim->cmds.cmd->handlers_num == 0)
//                 {
//                     // If all handlers were executed then return with ok code and 'delete' cmd from listener
//                     // sim->cmds.cmd->err = err;
//                     /* EDIT */
//                     xQueueSend(sim->cmds.queue, (void *)&err /*&sim->cmds.cmd->resp.err*/, portMAX_DELAY);
//                     /********/
//                 }

//                 if (sim->rec_len == 0)
//                 {
//                     // no message left
//                     sim->unread_num = 0;
//                     goto REGION_END;
//                 }
//                 else
//                 {
//                     // another message, or part of it left in buffer, let it be processed by another cmd
//                 }
//             }
//             else if (err == SIM_noErrCode || err == SIM_noResp)
//             {
//                 // try again untill the cmd isn't removed
//                 goto REGION_END;
//             }
//             else
//             {
//                 // ecountered error
//                 SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message                /* EDIT */
//                 xQueueSend(sim->cmds.queue, (void *)&err, portMAX_DELAY);
//                 /********/

//                 if (sim->rec_len == 0)
//                 {
//                     // no message left
//                     sim->unread_num = 0;
//                     goto REGION_END;
//                 }
//                 else
//                 {
//                     // another message, or part of it left in buffer, let it be processed by another cmd
//                 }
//             }

//             break;
//         }
//         case SIM_cmd_multiple_launch:
//         {

//             err = sim->cmds.cmd->handlers[sim->cmds.cmd->handlers_num - 1](sim->buf, sim->rec_len, &sim->cmds.cmd->resp, (void *)sim);

//             if (err == SIM_ok)
//             {
//                 // Handler was executed correctly
//                 SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
//                 /* EDIT */
//                 xQueueSend(sim->cmds.queue, (void *)&sim->cmds.cmd->resp.err, portMAX_DELAY);
//                 /********/

//                 if (sim->rec_len == 0)
//                 {
//                     // no message left
//                     sim->unread_num = 0;
//                     goto REGION_END;
//                 }
//                 else
//                 {
//                     // another message, or part of it left in buffer, let it be processed by another cmd
//                 }
//             }
//             else if (err == SIM_noErrCode || err == SIM_noResp)
//             {
//                 // didn't match, check another cmd
//             }
//             else
//             {
//                 // encountered error
//                 SIM_setMsg(sim->buf, &sim->rec_len, sim->cmds.cmd->resp.msg_end); // rewrite buffer so it contains only the unread part of the message
//                 /* EDIT */
//                 xQueueSend(sim->cmds.queue, (void *)&err, portMAX_DELAY);
//                 /********/
//                 // 'delete' cmd from listener
//                 // sim->cmds.cmd = NULL;

//                 if (sim->rec_len == 0)
//                 {
//                     // no message left
//                     sim->unread_num = 0;
//                     goto REGION_END;
//                 }
//                 else
//                 {
//                     // another message, or part of it left in buffer, let it be processed by another cmd
//                 }
//             }

//             break;
//         }
//         default:
//         {
//             break;
//         }
//         }
//     }

// REGION_END:
//     xSemaphoreGive(sim->exec_mutex);
//     return err;
// }

LL_SIM_error SIM_wait(LL_SIM_intf *sim, SIM_time time)
{
    LL_SIM_error err = SIM_timeoutErr;

    xQueueReceive(sim->cmds.queue, &err, time / portTICK_PERIOD_MS);

    return err;
}

void LL_SIM_listen(LL_SIM_intf *sim)
{
    // read data non stop
    xTaskCreate(SIM_receiveHandler, "SIM_listener", 5000, (void *)sim, 5, NULL);
}

void SIM_receiveHandler(void *sim_void)
{
    LL_SIM_intf *sim = sim_void;

    for (;;)
    {
        LL_SIM_receiveRaw(sim);
        SIM_exec(sim);
    }
}

// LL_SIM_error reinit()
// {
//     if(deinit() == LL_SIMOk)
//         return init();
//     else
//         return LL_SIMErr;
// }

/* Send data to LL_SIM */
LL_SIM_error LL_SIM_sendData(const LL_SIM_intf *sim, void *data, const unsigned int data_len)
{
    LL_SIM_error err = SIM_ok;
    xSemaphoreTake(sim->write_mutex, portMAX_DELAY);
    /* Get rid of any data left */
    uart_flush_input(sim->uart);
    xQueueReset(sim->uartQueue);

    /* Send data */
    int len = uart_write_bytes(sim->uart, data, data_len);

    if (len > 0)
        err = SIM_ok;
    else if (len == 0)
        err = SIM_err;
    else
        err = SIM_hardwareErr;

    xSemaphoreGive(sim->write_mutex);
    return err;
}

// void SIM_receiveAny(LL_SIM_intf *sim)
// {

//     xSemaphoreTake(sim->add_cmd_mutex, portMAX_DELAY);
// }

/*  Receive data from LL_SIM using ESP interrupts.
 *   Detects every new line or timeout.
 */
// #define PATTERN_CHR_NUM 1

SIM_data_len LL_SIM_receiveRaw(LL_SIM_intf *sim)
{
    uart_event_t event;
    // size_t buffered_size;
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

    while (xQueueReceive(sim->uartQueue, (void *)&event, portMAX_DELAY))

    {
        // bzero(sim->buf, sim->buf_len);
        ESP_LOGI(TAG, "uart[%d] event:", sim->uart);
        switch (event.type)
        {
        // Event of UART receving data
        case UART_DATA:
        {
            // ESP_LOGI(TAG, "[UART DATA]: %d", event.size); // TODO delete??
            if (sim->unread_num == 0)
                sim->rec_len = 0;

            sim->rec_len += uart_read_bytes(sim->uart, sim->buf + sim->rec_len, event.size, portMAX_DELAY);
            (*((char *)(sim->buf) + sim->rec_len)) = '\0';
            sim->unread_num++;
            // ESP_LOGI(TAG, "[UART DATA]: %d", sim->rec_len);
            // printf("%s", sim->buf); // TODO delete
            if (sim->rec_len > 0)
                return sim->rec_len;
            else if (sim->rec_len == 0)
                return SIM_recErr;
            else
                return SIM_hardwareErr;
            break;
        }
        // Event of HW FIFO overflow detected
        case UART_FIFO_OVF:
            ESP_LOGI(TAG, "hw fifo overflow");
            uart_flush_input(sim->uart);
            xQueueReset(sim->uartQueue);
            break;
        // Event of UART ring buffer full
        case UART_BUFFER_FULL:
            ESP_LOGI(TAG, "ring buffer full");
            uart_flush_input(sim->uart);
            xQueueReset(sim->uartQueue);
            // TODO resize buffer
            return SIM_bufferFullErr;
            break;
        // Event of UART RX break detected
        case UART_BREAK:
            ESP_LOGI(TAG, "uart rx break");
            break;
        // Event of UART parity check error
        case UART_PARITY_ERR:
            ESP_LOGI(TAG, "uart parity error");
            break;
        // Event of UART frame error
        case UART_FRAME_ERR:
            ESP_LOGI(TAG, "uart frame error");
            break;
        case UART_DATA_BREAK:
            ESP_LOGI(TAG, "uart data break");
            break;
        case UART_EVENT_MAX:
            ESP_LOGI(TAG, "uart event max");
            break;
        // UART_PATTERN_DET
        case UART_PATTERN_DET:
        {
            // uart_get_buffered_data_len(uart, &buffered_size);
            // int pos = uart_pattern_pop_pos(uart);
            // ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
            // if (pos == -1)
            // {
            //     uart_flush_input(uart);
            // }
            // else
            // {
            //     uart_read_bytes(uart, receivedData, pos, 100 / portTICK_PERIOD_MS);
            //     uint8_t pat[PATTERN_CHR_NUM + 1];
            //     memset(pat, 0, sizeof(pat));
            //     uart_read_bytes(uart, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
            //     ESP_LOGI(TAG, "read data: %s", receivedData);
            //     ESP_LOGI(TAG, "read pat : %s", pat);
            // }
            break;
        }
        // Others
        default:
            ESP_LOGI(TAG, "uart event type: %d", event.type);
            // break;
        }
    }

    return (int)SIM_recErr;
}

void LL_SIM_delay(int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

LL_SIM_error LL_SIM_setDRT(const LL_SIM_intf *sim, LL_SIM_pin set)
{
    if (gpio_set_level(sim->drt, set) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}

LL_SIM_error LL_SIM_setRST(const LL_SIM_intf *sim, LL_SIM_pin set)
{
    if (gpio_set_level(sim->rst, set) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}
