#include <uart_deamon.h>

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <driver/uart.h>
#include <esp_log.h>
/* Main deamon */
#include <error_handler.h>

void uart_change_conf(void *uart_handler_ptr, uart_sett sett, void *arg)
{
    uart_deamon_handler *handler = (uart_deamon_handler *)uart_handler_ptr;

    switch (sett)
    {
    case uart_sett_baud_rate:
    {
        /* Set UART */
        handler->uart_conf.baud_rate = *((int *)arg);

        if (
            uart_set_baudrate(UART_DAEMON_DEF_UART_NUM, handler->uart_conf.baud_rate) ||
            uart_flush(UART_DAEMON_DEF_UART_NUM) ||
            uart_flush_input(UART_DAEMON_DEF_UART_NUM))
        {
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, uart_daemon_hardware_err);
        }

        break;
    }
    case uart_sett_char_size:
    {
        handler->uart_conf.data_bits = *((int *)arg);

        if (
            uart_set_word_length(UART_DAEMON_DEF_UART_NUM, handler->uart_conf.data_bits) ||
            uart_flush(UART_DAEMON_DEF_UART_NUM) ||
            uart_flush_input(UART_DAEMON_DEF_UART_NUM))
        {
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, uart_daemon_hardware_err);
        }

        break;
    }
    case uart_sett_parity:
    {
        switch (*arg)
        {
        case PARITY_EVEN_C:
        {
            handler->uart_conf.parity = UART_PARITY_EVEN;
            break;
        }
        case PARITY_ODD_C:
        {
            handler->uart_conf.parity = UART_PARITY_ODD;
            break;
        }
        case PARITY_NONE_C:
        {
            handler->uart_conf.parity = UART_PARITY_DISABLE;
            break;
        }
        }

        /* Change parity */
        if (
            uart_set_parity(UART_DAEMON_DEF_UART_NUM, handler->uart_conf.parity) ||
            uart_flush(UART_DAEMON_DEF_UART_NUM) ||
            uart_flush_input(UART_DAEMON_DEF_UART_NUM))
        {
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, uart_daemon_hardware_err);
        }

        break;
    }
    case uart_sett_stop_bits:
    {
        switch (*arg)
        {
        case STOP_BITS_ONE_C:
        {
            handler->uart_conf.stop_bits = UART_STOP_BITS_1;

            break;
        }
        case STOP_BITS_ONEPOINTFIVE_C:
        {
            handler->uart_conf.stop_bits = UART_STOP_BITS_1_5;

            break;
        }
        case STOP_BITS_TWO_C:
        {
            handler->uart_conf.stop_bits = UART_STOP_BITS_2;

            break;
        }
        }

        /* Change parity */
        if (
            uart_set_stop_bits(UART_DAEMON_DEF_UART_NUM, handler->uart_conf.stop_bits) ||
            uart_flush(UART_DAEMON_DEF_UART_NUM) ||
            uart_flush_input(UART_DAEMON_DEF_UART_NUM))
        {
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, uart_daemon_hardware_err);
        }

        break;
    }
    }
}

int uart_write(unsigned char *buf, size_t len)
{
    return uart_write_bytes(UART_DAEMON_DEF_UART_NUM, buf, len);
}

void uart_deamon(void *v_handler)
{
    uart_deamon_handler *handler = (uart_deamon_handler *)v_handler;
    QueueHandle_t *queue = &handler->queue;
    uart_event_t event;
    int len;
    unsigned char buf[UART_DAEMON_DEF_REC_BUF_SIZE];

    while (xQueueReceive(*queue, &event, portMAX_DELAY))
    {
        switch (event.type)
        {
        case UART_DATA:
        {
            len += uart_read_bytes(UART_DAEMON_DEF_UART_NUM, buf, event.size, portMAX_DELAY);

            if (len > 0)
            {
                handler->resp_handler(buf, len);
            }

            break;
        }
        case UART_FIFO_OVF:
        {
            ESP_LOGI(TAG, "hw fifo overflow");
            uart_flush_input(UART_DAEMON_DEF_UART_NUM);
            xQueueReset(*queue);
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, UART_FIFO_OVF);
            break;
        }
        case UART_BUFFER_FULL:
        {
            ESP_LOGI(TAG, "ring bufer full");
            uart_flush_input(UART_DAEMON_DEF_UART_NUM);
            xQueueReset(*queue);
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, UART_BUFFER_FULL);
            break;
        }
        case UART_BREAK:
        {
            ESP_LOGI(TAG, "uart rx break");
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_non_fatal, UART_BREAK);
            break;
        }
        case UART_PARITY_ERR:
        {
            ESP_LOGI(TAG, "uart parity error");
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, UART_PARITY_ERR);
            break;
        }
        case UART_FRAME_ERR:
        {
            ESP_LOGI(TAG, "uart frame error");
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, UART_FRAME_ERR);
            break;
        }
        case UART_DATA_BREAK:
        {
            ESP_LOGI(TAG, "uart data break");
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_non_fatal, UART_DATA_BREAK);
            break;
        }
        case UART_EVENT_MAX:
        {
            handler->error_handler(&handler->handler, UART_DAEMON_TASK_NAME, ext_type_fatal, UART_DATA_BREAK);
            break;
        }
        case UART_PATTERN_DET:
        {
            break;
        }
        default:
            break;
        }
    }

    /* wait for the task to be deleted */
    for (;;)
        ;
}

void uart_deamon_reinit(
    uart_deamon_handler handler,
    void *mqtt_handler,
    void (*resp_handler)(void *, unsigned char *, unsigned int),
    void (*error_handler)(void *handler, const char *module, int type, int err))
{
    handler->uart_conf = uart_deamon_load_config();
    handler->mqtt_handler = mqtt_handler;
    handler->resp_handler = resp_handler;
    handler->error_handler = error_handler;
}

TaskHandle_t uart_deamon_create_task(uart_deamon_handler *handler)
{
    if (!(handler->handler))
    {
        xTaskCreate(uart_deamon, UART_DAEMON_TASK_NAME, UART_DAEMON_STACK_SIZE, (void *)handler, UART_DAEMON_TASK_PRIORITY, &handler->handler);
    }
    return handler->handler;
}

TaskHandle_t uart_deamon_delete_task(uart_deamon_handler *handler)
{
    if (handler->handler)
    {
        vTaskDelete(handler->handler);
        handler->handler = NULL;
    }
    return handler->handler;
}

uart_config_t uart_deamon_load_config()
{
    // TODO load config from memory
    uart_config_t uart_def_conf = {
        .baud_rate = UART_DAEMON_DEF_UART_BAUD_RATE,
        .data_bits = UART_DAEMON_DEF_UART_DATA_BITS,
        .parity = UART_DAEMON_DEF_UART_PARITY,
        .stop_bits = UART_DAEMON_DEF_UART_STOP_BITS,
        .flow_ctrl = UART_DAEMON_DEF_UART_FLOW_CTRL,
        .rx_flow_ctrl_thresh = UART_DAEMON_DEF_UART_RX_FLOW_CTRL_TRESH,
        .source_clk = UART_DAEMON_DEF_UART_SOURCE_CLK};

    return uart_def_conf;
}

uart_config_t *uart_deamon_save_config(uart_config_t *new_uart_conf)
{
    // uart_config_t *uart_conf = uart_deamon_get_config();

    // TODO save to nvs

    return new_uart_conf;
}

/* Start UART deamon.
 * If driver install and starting deamon was successful
 * then handler != NULL and returned value == 0.
 * If driver install and uninstall was successful but starting deamon was't
 * then handler == NULL and returned value == 0.
 * If both we unsuccessfull
 * then handler == NULL and returned value != 0.*/
int uart_deamon_start(uart_deamon_handler *handler)
{
    esp_err_t err;

    /* Basic UART config */
    if ((err = uart_driver_install(UART_DAEMON_DEF_UART_NUM,
                                   UART_DAEMON_DEF_RX_BUF_SIZE,
                                   UART_DAEMON_DEF_TX_BUF_SIZE,
                                   UART_DAEMON_DEF_QUEUE_SIZE,
                                   &handler->queue,
                                   0)) != ESP_OK)
    {
        /* Driver installation failed */
        return (int)err; //(int)err;
    }

    if ((err = uart_param_config(UART_DAEMON_DEF_UART_NUM,
                                 &handler->uart_conf)) != ESP_OK)
    {
        /* Parameter configuration failed, uninstall drivers */
        err = uart_driver_delete(UART_DAEMON_DEF_UART_NUM);
        return (int)err;
    }
    ESP_ERROR_CHECK(uart_set_pin(UART_DAEMON_DEF_UART_NUM, UART_DAEMON_DEF_UART_TX, UART_DAEMON_DEF_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Finally run the deamon */
    uart_deamon_create_task(handler);
    if (handler->handler)
    {
        /* OK */
        return (int)err;
    }
    else
    {
        /* Deamon creation failed, uninstall drivers */
        err = uart_driver_delete(UART_DAEMON_DEF_UART_NUM);
        return (int)err;
    }
}

int uart_deamon_stop(uart_deamon_handler *handler)
{
    esp_err_t err = ESP_OK;

    /* Stop UART deamon */
    uart_deamon_delete_task(handler);

    if (!(handler->handler))
    {
        err = uart_driver_delete(UART_DAEMON_DEF_UART_NUM);
        return (int)err;
    }
    else
    {
        return (int)err;
    }
}

// TaskHandle_t *uart_deamon_get_task()
// {
//     static TaskHandle_t handle = NULL;
//     return &handle;
// }
