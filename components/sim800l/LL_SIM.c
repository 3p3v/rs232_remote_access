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

void LL_SIM_def(LL_SIM_int *sim)
{
    memset(sim->at, '\0', SIM_MAX_AT_STR_LEN);
    sim->buf = NULL;
    sim->buf_len = 0;
    sim->rec_len = 0;
    sim->tx = LL_SIM_DEF_UART_TX;
    sim->rx = LL_SIM_DEF_UART_RX;
    sim->baudRate = 9600;
    sim->drt = LL_SIM_DEF_DRT_PIN;
    sim->rst = LL_SIM_DEF_RST_PIN;
    sim->uart = LL_SIM_DEF_UART_NUM;
    sim->uartQueue = NULL;//xQueueCreate(LL_SIM_DEF_QUEUE_SIZE, sizeof(int));
}

/*  UART configuration, includes ESP pattern detection (detects every new line) */
LL_SIM_error LL_SIM_init(const LL_SIM_int *sim)
{
    /* UART */
    uart_config_t uartConf = {
	    .baud_rate = sim->baudRate,
	    .data_bits = UART_DATA_8_BITS,
	    .parity    = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT
	};

    // uartQueue = 

    /* Basic UART config */
    ESP_ERROR_CHECK(uart_driver_install(sim->uart, LL_SIM_DEF_BUF_SIZE, LL_SIM_DEF_BUF_SIZE, LL_SIM_DEF_QUEUE_SIZE, &sim->uartQueue, 0));
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
        .intr_type = GPIO_INTR_DISABLE
    };

    /* Set DRT and RST pin */
    ESP_ERROR_CHECK(gpio_config(&gpioConf));

    // resetForce();

    // setStatus(LL_SIMUARTInitialised);

    return SIM_ok;
}

LL_SIM_error LL_SIM_deinit(const LL_SIM_int *sim)
{
    // resetStatus(LL_SIMUARTInitialised);
    if(uart_driver_delete(sim->uart) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}

// LL_SIM_error reinit()
// {
//     if(deinit() == LL_SIMOk)
//         return init();
//     else    
//         return LL_SIMErr;
// }

/* Send data to LL_SIM */
LL_SIM_error LL_SIM_sendData(const LL_SIM_int* sim, const char *data)
{
    /* Get rid of any data left */
    uart_flush_input(sim->uart);
    xQueueReset(sim->uartQueue);

    /* Send data */
    int len = uart_write_bytes(sim->uart, data, strlen(data));

    if (len > 0)
        return SIM_ok;
    else if (len == 0)
        return SIM_err;
    else 
        return SIM_hardwareErr;
}

/*  Receive data from LL_SIM using ESP interrupts.
 *   Detects every new line or timeout.
 */
// #define PATTERN_CHR_NUM 1

SIM_data_len LL_SIM_receiveRaw(const LL_SIM_int* sim, SIM_time time)
{
    uart_event_t event;
    // size_t buffered_size;
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

    while(((xTaskGetTickCount() * portTICK_PERIOD_MS) - startTime) < time) {
        if (xQueueReceive(sim->uartQueue, (void *)&event, time / portTICK_PERIOD_MS))
        {
            bzero(sim->buf, sim->buf_len);
            ESP_LOGI(TAG, "uart[%d] event:", sim->uart);
            switch (event.type)
            {
            // Event of UART receving data
            case UART_DATA: {
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size); //TODO delete??
                int len = uart_read_bytes(sim->uart, sim->buf, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[UART DATA]: %d", len);
                printf("%s", sim->buf); //TODO delete
                if (len > 0)
                    return len;
                else if (len == 0) 
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
                //TODO resize buffer
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
        } else {
            ESP_LOGI(TAG, "timeout");
            return (int)SIM_timeoutErr; 
        }
    }

    return (int)SIM_recErr; 
}

SIM_data_len LL_SIM_receiveRaw_checkCustomErr(const LL_SIM_int* sim, SIM_time time, SIM_error (*check)(const LL_SIM_int *, const SIM_err_pair *), const SIM_err_pair *err)
{
    uart_event_t event;
    // size_t buffered_size;
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

    while(((xTaskGetTickCount() * portTICK_PERIOD_MS) - startTime) < time) {
        if (xQueueReceive(sim->uartQueue, (void *)&event, time / portTICK_PERIOD_MS))
        {
            bzero(sim->buf, sim->buf_len);
            ESP_LOGI(TAG, "uart[%d] event:", sim->uart);
            switch (event.type)
            {
            // Event of UART receving data
            case UART_DATA: {
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size); //TODO delete??
                int len = uart_read_bytes(sim->uart, sim->buf, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[UART DATA]: %d", len);
                printf("%s", sim->buf); //TODO delete
                if (len > 0) {
                    if(check(sim, err) == SIM_ok)
                        return len;
                    else
                        break;
                }
                else if (len == 0) 
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
                //TODO resize buffer
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
        } else {
            ESP_LOGI(TAG, "timeout");
            return (int)SIM_timeoutErr; 
        }
    }

    return (int)SIM_recErr; 
}

SIM_data_len LL_SIM_receiveRaw_checkErr(const LL_SIM_int* sim, SIM_time time, SIM_error (*check)(const LL_SIM_int *))
{
    uart_event_t event;
    // size_t buffered_size;
    int startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

    while(((xTaskGetTickCount() * portTICK_PERIOD_MS) - startTime) < time) {
        if (xQueueReceive(sim->uartQueue, (void *)&event, time / portTICK_PERIOD_MS))
        {
            bzero(sim->buf, sim->buf_len);
            ESP_LOGI(TAG, "uart[%d] event:", sim->uart);
            switch (event.type)
            {
            // Event of UART receving data
            case UART_DATA: {
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size); //TODO delete??
                int len = uart_read_bytes(sim->uart, sim->buf, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[UART DATA]: %d", len);
                printf("%s", sim->buf); //TODO delete
                if (len > 0) {
                    if(check(sim) == SIM_ok)
                        return len;
                    else
                        break;
                }
                else if (len == 0) 
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
                //TODO resize buffer
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
        } else {
            ESP_LOGI(TAG, "timeout");
            return (int)SIM_timeoutErr; 
        }
    }

    return (int)SIM_recErr; 
}

void LL_SIM_delay(int ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

LL_SIM_error LL_SIM_setDRT(const LL_SIM_int* sim, LL_SIM_pin set) {
    if(gpio_set_level(sim->drt, set) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}

LL_SIM_error LL_SIM_setRST(const LL_SIM_int* sim, LL_SIM_pin set) {
    if(gpio_set_level(sim->rst, set) == ESP_OK)
        return SIM_ok;
    else
        return SIM_hardwareErr;
}
