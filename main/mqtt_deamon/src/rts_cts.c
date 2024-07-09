#include <rts_cts.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <mqtt_queue.h>

IRAM_ATTR static void cts_handle(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    
    int intr_type = CTS_CHANGED_STATE;
    QueueHandle_t queue = (QueueHandle_t)arg;

    xQueueSendFromISR(queue, &intr_type, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR(NULL);
    }
}

IRAM_ATTR static void rts_handle(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    
    int intr_type = RTS_CHANGED_STATE;
    QueueHandle_t queue = (QueueHandle_t)arg;

    xQueueSendFromISR(queue, &intr_type, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR(NULL);
    }
}

void rts_cts_set_mode(dev_mode mode, QueueHandle_t *queue)
{
    /* Reset gpio */
    gpio_reset_pin(MQTT_CTS_PIN);
    gpio_reset_pin(MQTT_RTS_PIN);

    /* GPIO RTS, CTS */
    const gpio_config_t cts_config = {
        .pin_bit_mask = BIT(MQTT_CTS_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE};
    gpio_config(&cts_config);

    const gpio_config_t rts_config = {
        .pin_bit_mask = BIT(MQTT_RTS_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE};
    gpio_config(&rts_config);

    gpio_set_intr_type(MQTT_RTS_PIN, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);

    if (mode == dte)
    {
        gpio_isr_handler_add(MQTT_RTS_PIN, cts_handle, (void *)queue);
    }
    else
    {
        gpio_isr_handler_add(MQTT_RTS_PIN, cts_handle, (void *)queue);
    }
}