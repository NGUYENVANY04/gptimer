#include <stdio.h>
#include "timer_handle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include <sys/time.h>
#include <esp_system.h>
#include "driver/gpio.h"

#define TAG "app"
bool led_on = true;

gptimer_handle_t gptimer_1 = NULL;
gptimer_handle_t gptimer_2 = NULL;

static bool IRAM_ATTR handle_timer_1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{

    ESP_ERROR_CHECK(gptimer_start(gptimer_2));

    return true;
}
static bool IRAM_ATTR handle_timer_2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_ERROR_CHECK(gptimer_start(gptimer_1));

    return true;
}

void setup_timer_1(uint64_t timer_1_duration_us)
{
    ESP_LOGI(TAG, "Creating timer handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_1));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_1,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_1, &cbs, NULL));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_1));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_1_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_1_duration_us,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_1, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer_1));
}
void setup_timer_2(uint64_t timer_2_duration_us)
{
    ESP_LOGI(TAG, "Creating timer handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_2));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_2,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_2, &cbs, NULL));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_2));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_2_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_2_duration_us,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_2, &alarm_config));
}