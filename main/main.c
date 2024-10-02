#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "function_keys.h"
#include "handle_isr.h"

// static const char *TAG = "example";

typedef struct
{
    uint64_t event_count;
} example_queue_element_t;

// Callback function to handle timer alarm event
// static bool IRAM_ATTR handle(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
// {
//     BaseType_t high_task_awoken = pdFALSE;
//     QueueHandle_t queue = (QueueHandle_t)user_data;
//     example_queue_element_t ele = {
//         .event_count = edata->count_value};
//     xQueueSendFromISR(queue, &ele, &high_task_awoken);
//     gpio_set_level(GPIO_NUM_2, 1);
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//     gpio_set_level(GPIO_NUM_2, 0);

//     // return whether we need to yield at the end of ISR
//     return (high_task_awoken == pdTRUE);
// }

void app_main(void)
{
    // example_queue_element_t ele;
    // QueueHandle_t queue = xQueueCreate(10, sizeof(example_queue_element_t));
    // if (!queue)
    // {
    //     ESP_LOGE(TAG, "Creating queue failed");
    //     return;
    // }

    // gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    // ESP_LOGI(TAG, "Create timer handle");
    // gptimer_handle_t gptimer = NULL;
    // gptimer_config_t timer_config = {
    //     .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    //     .direction = GPTIMER_COUNT_UP,
    //     .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    // };
    // ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // gptimer_event_callbacks_t cbs = {
    //     .on_alarm = handle,
    // };
    // ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));

    // ESP_LOGI(TAG, "Enable timer");
    // ESP_ERROR_CHECK(gptimer_enable(gptimer));

    // ESP_LOGI(TAG, "Start timer for 5 seconds");
    // gptimer_alarm_config_t alarm_config = {
    //     .alarm_count = 5000000, // period = 5s (5,000,000 microseconds)
    //     .reload_count = 0,
    //     .flags.auto_reload_on_alarm = true,
    // };
    // ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    // ESP_ERROR_CHECK(gptimer_start(gptimer));

    // // // Wait for the alarm event to trigger
    // // if (xQueueReceive(queue, &ele, pdMS_TO_TICKS(6000)))
    // // {
    // //     ESP_LOGI(TAG, "Timer alarm event received, count=%llu", ele.event_count);
    // // }
    // // else
    // // {
    // //     ESP_LOGW(TAG, "Missed the timer alarm event");
    // // }

    // // // Stop and disable the timer after the event
    // // ESP_LOGI(TAG, "Stop timer");
    // // ESP_ERROR_CHECK(gptimer_stop(gptimer));
    // // ESP_LOGI(TAG, "Disable timer");
    // // ESP_ERROR_CHECK(gptimer_disable(gptimer));
    // // ESP_LOGI(TAG, "Delete timer");
    // // ESP_ERROR_CHECK(gptimer_del_timer(gptimer));

    // // Cleanup
    // vQueueDelete(queue);
    init_setting();
}
