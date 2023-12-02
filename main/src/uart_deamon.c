#include <uart_deamon.h>

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <driver/uart.h>
#include <esp_log.h>
/* MQTT-C lib */
#include <MQTTClient.h>
/* Main deamon */
#include <error_handler.h>
#include <mqtt_deamon.h>

static const char *TAG = "UART_DEAMON";

/* GPIO interrupt handler */
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
}

int uart_write(unsigned char *buf, size_t len)
{
    return uart_write_bytes(UART_DEAMON_DEF_UART_NUM, buf, len);
}

void uart_deamon(void *v_handler)
{
    // esp_err_t err = ESP_OK;
    uart_deamon_handler *handler = (uart_deamon_handler *)v_handler;
    QueueHandle_t *queue = &handler->queue;
    uart_event_t event;
    int len;
    unsigned char buf[UART_DEAMON_DEF_REC_BUF_SIZE];

    while (xQueueReceive(*queue, &event, portMAX_DELAY))
    {
        switch (event.type)
        {
        // Event of UART receving data
        case UART_DATA:
        {
            ESP_LOGI(TAG, "uart[%d] event size: %i", UART_DEAMON_DEF_UART_NUM, event.size);

            len += uart_read_bytes(UART_DEAMON_DEF_UART_NUM, buf, event.size, portMAX_DELAY);

            if (len > 0)
            {
                handler->resp_handler(buf, len);
            }

            len = 0;

            break;
        }
        // Event of HW FIFO overflow detected
        case UART_FIFO_OVF:
            ESP_LOGI(TAG, "hw fifo overflow");
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            xQueueReset(*queue);
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_FIFO_OVF);
            break;
        // Event of UART ring buffer full
        case UART_BUFFER_FULL:
            ESP_LOGI(TAG, "ring bufer full");
            uart_flush_input(UART_DEAMON_DEF_UART_NUM);
            xQueueReset(*queue);
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_BUFFER_FULL);
            // return SIM_TAGferFullErr;
            break;
        // Event of UART RX break detected
        case UART_BREAK:
            ESP_LOGI(TAG, "uart rx break");
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_BREAK);
            break;
        // Event of UART parity check error
        case UART_PARITY_ERR:
            ESP_LOGI(TAG, "uart parity error");
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_PARITY_ERR);
            break;
        // Event of UART frame error
        case UART_FRAME_ERR:
            ESP_LOGI(TAG, "uart frame error");
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_FRAME_ERR);
            break;
        case UART_DATA_BREAK:
            ESP_LOGI(TAG, "uart data break");
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_DATA_BREAK);
            break;
        case UART_EVENT_MAX:
            ESP_LOGI(TAG, "uart event max");
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, UART_EVENT_MAX);
            break;
        case UART_PATTERN_DET:
        {
            break;
        }
        default:
            ESP_LOGI(TAG, "uart event type: %d", event.type);
            handler->error_handler(&handler->handler, "UART", ext_type_non_fatal, event.type);
            break;
        }
    }

    /* wait for the task to be deleted */
    for (;;)
        ;
}

TaskHandle_t uart_deamon_create_task(uart_deamon_handler *handler)
{
    if (!(handler->handler))
    {
        xTaskCreate(uart_deamon, "uart_deamon", 12000, (void *)handler, 3, &handler->handler);
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
        .baud_rate = UART_DEAMON_DEF_UART_BAUD_RATE,
        .data_bits = UART_DEAMON_DEF_UART_DATA_BITS,
        .parity = UART_DEAMON_DEF_UART_PARITY,
        .stop_bits = UART_DEAMON_DEF_UART_STOP_BITS,
        .flow_ctrl = UART_DEAMON_DEF_UART_FLOW_CTRL,
        .rx_flow_ctrl_thresh = UART_DEAMON_DEF_UART_RX_FLOW_CTRL_TRESH,
        .source_clk = UART_DEAMON_DEF_UART_SOURCE_CLK};

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
    if ((err = uart_driver_install(UART_DEAMON_DEF_UART_NUM,
                                   UART_DEAMON_DEF_RX_BUF_SIZE,
                                   UART_DEAMON_DEF_TX_BUF_SIZE,
                                   UART_DEAMON_DEF_QUEUE_SIZE,
                                   &handler->queue,
                                   0)) != ESP_OK)
    {
        /* Driver installation failed */
        return (int)err; //(int)err;
    }

    if ((err = uart_param_config(UART_DEAMON_DEF_UART_NUM,
                                 &handler->uart_conf)) != ESP_OK)
    {
        /* Parameter configuration failed, uninstall drivers */
        err = uart_driver_delete(UART_DEAMON_DEF_UART_NUM);
        return (int)err;
    }
    ESP_ERROR_CHECK(uart_set_pin(UART_DEAMON_DEF_UART_NUM, UART_DEAMON_DEF_UART_TX, UART_DEAMON_DEF_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

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
        err = uart_driver_delete(UART_DEAMON_DEF_UART_NUM);
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
        err = uart_driver_delete(UART_DEAMON_DEF_UART_NUM);
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
