#include <mqtt_ping.h>
#include <driver/gptimer.h>
#include <mqtt_deamon.h>

/* Ping callback */
IRAM_ATTR static bool ping_handle(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    int val = PING_REQUEST;
    xQueueSendFromISR(queue, &val, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR(NULL);
    }

    return xHigherPriorityTaskWoken;
}

void start_ping_timer(mqtt_deamon_handler *handler)
{
    gptimer_config_t t_conf = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = PING_TIMER_RES // 1MHz, 1 tick = 1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&t_conf, &handler->ping_timer));
    /* Configure alarm time */
    gptimer_alarm_config_t t_alarm_conf = {
        .alarm_count = handler->keep_alive_int * PING_TIMER_RES,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true};
    ESP_ERROR_CHECK(gptimer_set_alarm_action(handler->ping_timer, &t_alarm_conf));
    /* Mount callback */
    gptimer_event_callbacks_t t_callb_conf = {
        .on_alarm = ping_handle};
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(handler->ping_timer, &t_callb_conf, handler->queue));
    ESP_ERROR_CHECK(gptimer_enable(handler->ping_timer));
    /* Start timer */
    ESP_ERROR_CHECK(gptimer_start(handler->ping_timer));
}

void reload_ping_timer(mqtt_deamon_handler *handler)
{
    gptimer_set_raw_count(handler->ping_timer, 0);
}

void stop_ping_timer(mqtt_deamon_handler *handler)
{
    /* Stop */
    gptimer_stop(handler->ping_timer);
    /* Disable callbacks */
    gptimer_disable(handler->ping_timer);
}