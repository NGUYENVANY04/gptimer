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
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "webserver.h"
#include "function_keys.h"

#define TAG "app"
bool flag_timer_1 = true;
bool flag_timer_2 = false;
bool flag_timer_3 = false;
bool restart_timer = false;
gptimer_handle_t gptimer_1 = NULL; // clock 1
gptimer_handle_t gptimer_2 = NULL; // clock  2
gptimer_handle_t gptimer_3 = NULL; // clock  3
gptimer_handle_t gptimer_4 = NULL; // led 4 server

gptimer_handle_t gptimer_stop_wifi = NULL;
TaskHandle_t stop_task_handle = NULL;
static bool IRAM_ATTR handle_timer_1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    flag_timer_2 = true; // start run timer 2
    gpio_set_level(PIN_RELAY_1, 1);
    gpio_set_level(PIN_RELAY_2, 1);
    ESP_EARLY_LOGI("Check flow", "Timer 2 started , relay 1 , 2 started");
    return true;
}

static bool IRAM_ATTR handle_timer_2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer  3 started , relay 2 continiuuu, 3 started ");
    gpio_set_level(PIN_RELAY_3, 1);
    flag_timer_3 = true;
    return true;
}
static bool IRAM_ATTR handle_timer_3(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer 3 stoped , relay 1,3  stoped");
    flag_timer_1 = true;
    gpio_set_level(PIN_RELAY_2, 0);
    gpio_set_level(PIN_RELAY_3, 0);
    esp_restart();
    return true;
}
static bool IRAM_ATTR handle_timer_4(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer 4 stoped , relay 2  stoped");

    gpio_set_level(PIN_RELAY_2, 0);
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
}
void setup_timer_2(uint64_t timer_2_duration_us) // timer congfi by button left
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
void setup_timer_3(uint64_t timer_3_duration_us) // timer congfi by button right
{
    ESP_LOGI(TAG, "Creating timer 3 handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_3));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_3,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_3, &cbs, NULL));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_3));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_3_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_3_duration_us,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_3, &alarm_config));
}
void setup_timer_4(uint64_t timer_4_duration_us) // timer congfi by webserver
{
    ESP_LOGI(TAG, "Creating timer 4 handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_4));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_4,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_4, &cbs, NULL));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_4));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_4_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_4_duration_us,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = false,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_4, &alarm_config));
}

void check_flag_timer(void *par)
{
    while (true)
    {
        if (flag_timer_1)
        {
            flag_timer_1 = false;
            ESP_LOGI(TAG, "Starting timer 1");
            ESP_ERROR_CHECK(gptimer_start(gptimer_1));
        }
        if (flag_timer_2)
        {
            flag_timer_2 = false;
            ESP_LOGI(TAG, "Starting timer 2 and timer 4");
            ESP_ERROR_CHECK(gptimer_start(gptimer_2));
            ESP_ERROR_CHECK(gptimer_start(gptimer_4));
        }
        if (flag_timer_3)
        {
            flag_timer_3 = false;
            ESP_ERROR_CHECK(gptimer_start(gptimer_3));
        }
        // if (restart_timer)
        // {
        //     restart_timer = false;
        //     ESP_LOGI(TAG, "Starting timer 1");
        //     ESP_ERROR_CHECK(gptimer_start(gptimer_1));
        // }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
