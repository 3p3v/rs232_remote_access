#pragma once

/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <driver/uart.h>
#include <esp_log.h>
/* Main deamon */
#include <error_handler.h>
#include <mqtt_deamon.h>

/* UART config, no changable */
#define UART_DEAMON_DEF_UART_NUM UART_NUM_2
#define UART_DEAMON_DEF_UART_TX GPIO_NUM_4
#define UART_DEAMON_DEF_UART_RX GPIO_NUM_2
/* UART config, rewritten during first change of parameters */
#define UART_DEAMON_DEF_UART_BAUD_RATE 9600
#define UART_DEAMON_DEF_UART_DATA_BITS UART_DATA_8_BITS
#define UART_DEAMON_DEF_UART_PARITY UART_PARITY_DISABLE
#define UART_DEAMON_DEF_UART_STOP_BITS UART_STOP_BITS_1
#define UART_DEAMON_DEF_UART_FLOW_CTRL UART_HW_FLOWCTRL_DISABLE
#define UART_DEAMON_DEF_UART_RX_FLOW_CTRL_TRESH 0
#define UART_DEAMON_DEF_UART_SOURCE_CLK UART_SCLK_DEFAULT
/* UART buffers, no changable */
#define UART_DEAMON_DEF_TX_BUF_SIZE 1000
#define UART_DEAMON_DEF_RX_BUF_SIZE 1000
#define UART_DEAMON_DEF_REC_BUF_SIZE 1000
/* UART interrupt config, no changable */
#define UART_DEAMON_DEF_QUEUE_SIZE 60

static const char *TAG = "UART_DEAMON";

QueueHandle_t *uart_deamon_get_queue()
{
    static QueueHandle_t queue;
    return &queue;
}

// void uart_deamon_delete_queue()
// {
//     QueueHandle_t *queue = uart_deamon_get_queue();
//     vQueueDelete(*queue);
// }

void uart_deamon(void *)
{
    esp_err_t err = ESP_OK;
    QueueHandle_t *queue = uart_deamon_get_queue();
    uart_event_t event;
    int len;
    unsigned char buf[UART_DEAMON_DEF_REC_BUF_SIZE];

    while (xQueueReceive(*queue, &err, portMAX_DELAY))
    {
        switch (event.type)
        {
        // Event of UART receving data
        case UART_DATA:
        {
            ESP_LOGI(TAG, "uart[%d] event size:", UART_DEAMON_DEF_UART_NUM, event.size);

            len += uart_read_bytes(UART_DEAMON_DEF_UART_NUM, buf, event.size, portMAX_DELAY);
            buf[len] = '\0';
            
            if (len > 0)
            {
                mqtt_publish(&client, MQTT_TOPIC, buf, len, MQTT_PUBLISH_QOS_0);
                if (client.error != MQTT_OK)
                {
                    // TODO send error using ctrl channel
                }
                mqtt_deamon_awake();
            }
        }
        // Event of HW FIFO overflow detected
        case UART_FIFO_OVF:
            ESP_LOGI(TAG, "hw fifo overflow");
            uart_flush_input(sim->uart);
            xQueueReset(sim->uartQueue);
            // TODO send error using ctrl channel
            break;
        // Event of UART ring buffer full
        case UART_BUFFER_FULL:
            ESP_LOGI(TAG, "ring buffer full");
            uart_flush_input(sim->uart);
            xQueueReset(sim->uartQueue);
            // TODO send error using ctrl channel
            return SIM_bufferFullErr;
            break;
        // Event of UART RX break detected
        case UART_BREAK:
            ESP_LOGI(TAG, "uart rx break");
            break;
        // Event of UART parity check error
        case UART_PARITY_ERR:
            ESP_LOGI(TAG, "uart parity error");
            // TODO send error using ctrl channel
            break;
        // Event of UART frame error
        case UART_FRAME_ERR:
            ESP_LOGI(TAG, "uart frame error");
            // TODO send error using ctrl channel
            break;
        case UART_DATA_BREAK:
            ESP_LOGI(TAG, "uart data break");
            // TODO send error using ctrl channel
            break;
        case UART_EVENT_MAX:
            ESP_LOGI(TAG, "uart event max");
            // TODO send error using ctrl channel
            break;
        case UART_PATTERN_DET:
        {
            break;
        }
        default:
            ESP_LOGI(TAG, "uart event type: %d", event.type);
            // TODO send error using ctrl channel
            break;
        }
    }

    /* Send error to main deamon */
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    xQueueSend(*error_get_queue(), &task, portMAX_DELAY);

    /* wait for the task to be deleted */
    for (;;);
}

TaskHandle_t uart_deamon_create_task()
{
    TaskHandle_t *handle = uart_deamon_get_task();
    if (!(*handle))
    {
        xTaskCreate(uart_deamon, "uart_deamon", 4096, NULL, 3, handle);
    }
    return *handle;
}

TaskHandle_t uart_deamon_delete_task()
{
    TaskHandle_t *handle = uart_deamon_get_task();
    if (*handle)
    {
        vTaskDelete(*handle);
    }
    return *handle;
}

static uart_config_t uart_deamon_load_config()
{
    // TODO load config from memory
    uart_config_t uart_conf = {
        .baud_rate = UART_DEAMON_DEF_UART_BAUD_RATE,
        .data_bits = UART_DEAMON_DEF_UART_DATA_BITS,
        .parity = UART_DEAMON_DEF_UART_PARITY,
        .stop_bits = UART_DEAMON_DEF_UART_STOP_BITS,
        .flow_ctrl = UART_DEAMON_DEF_UART_FLOW_CTRL,
        .rx_flow_ctrl_thresh = UART_DEAMON_DEF_UART_RX_FLOW_CTRL_TRESH,
        .source_clk = UART_DEAMON_DEF_UART_SOURCE_CLK};

    return uart_conf;
}

uart_config_t *uart_deamon_get_config()
{
    static uart_config_t uart_conf = uart_deamon_load_config();
    return &uart_conf;
}

uart_config_t *uart_deamon_change_config(uart_config_t *new_uart_conf)
{
    uart_config_t *uart_conf = uart_deamon_get_config();
    *uart_conf = *new_uart_conf;

    // TODO load config to memory

    return uart_conf;
}

/* Start UART deamon.
 * If driver install and starting deamon was successful 
 * then handler != NULL and returned value == 0.
 * If driver install and uninstall was successful but starting deamon was't 
 * then handler == NULL and returned value == 0.
 * If both we unsuccessfull 
 * then handler == NULL and returned value != 0.*/
int uart_deamon_start(TaskHandle_t *handler, void (*rec_handler)(void *, ))
{
    esp_err_t err;
    *handler = NULL;

    /* UART-specyfic vars */
    uart_config_t *uart_conf = uart_deamon_get_config();
    QueueHandle_t *uart_queue = uart_deamon_get_queue();

    /* Basic UART config */
    if ((err = uart_driver_install(UART_DEAMON_DEF_UART_NUM,
                                   UART_DEAMON_DEF_RX_BUF_SIZE,
                                   UART_DEAMON_DEF_TX_BUF_SIZE,
                                   UART_DEAMON_DEF_QUEUE_SIZE,
                                   uart_queue,
                                   0)) != ESP_OK)
    {
        /* Driver installation failed */
        return ESP_OK;//(int)err;
    }

    if ((err = uart_param_config(UART_DEAMON_DEF_UART_NUM,
                                 uart_conf)) != ESP_OK)
    {
        /* Parameter configuration failed, uninstall drivers */
        err = uart_driver_delete(UART_DEAMON_DEF_UART_NUM);
        return (int)err;
    }

    /* Finally run the deamon */
    *handler = uart_deamon_create_task();
    if (*handler)
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

int uart_deamon_stop(TaskHandle_t *handler)
{
    esp_err_t err = ESP_OK;

    /* Stop UART deamon */
    *handler = uart_deamon_delete_task();

    if (!(*handler))
    {
        err = uart_driver_delete(UART_DEAMON_DEF_UART_NUM);
        return (int)err;
    }
    else
    {
        return (int)err;
    }
}

TaskHandle_t *uart_deamon_get_task()
{
    static TaskHandle_t handle = NULL;
    return &handle;
}
