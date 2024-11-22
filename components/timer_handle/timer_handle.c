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
#include "savedata.h"
#include "display_tm1637.h"

#define TAG "app"
bool flag_timer_1 = false;
bool on = true;
bool flag_timer_2 = false;
bool flag_timer_3 = false;
bool restart_timer = false;
bool relay_on = true;
gptimer_config_t timer_config = {
    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
    .direction = GPTIMER_COUNT_UP,
    .resolution_hz = 1000000,
};
gptimer_handle_t gptimer_1 = NULL;     // clock 1
gptimer_handle_t gptimer_2 = NULL;     // clock  2
gptimer_handle_t gptimer_3 = NULL;     // clock  3
gptimer_handle_t gptimer_delay = NULL; // led 4 server

gptimer_handle_t gptimer_stop_wifi = NULL;
TaskHandle_t stop_task_handle = NULL;
typedef struct
{
    bool free;
    uint64_t timer_3_duration_us_off;
    uint64_t timer_3_duration_us_on;
} timer_callback_data_t;
typedef struct
{
    int current_timer_1;
} timer_1_callback_data_t;
typedef struct
{
    int current_timer_2;
} timer_2_callback_data_t;
static bool IRAM_ATTR handle_timer_1(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    timer_1_callback_data_t *callback_data_timer_1 = (timer_1_callback_data_t *)user_data;
    callback_data_timer_1->current_timer_1--;
    ESP_EARLY_LOGI("Check flow", "%d", callback_data_timer_1->current_timer_1);
    tm1637_set_number(led1, callback_data_timer_1->current_timer_1);
    if (!callback_data_timer_1->current_timer_1)
    {
        flag_timer_2 = true;
    }
    if (callback_data_timer_1->current_timer_1 == 1)
    {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = 60000000,
            .reload_count = 0,
            .flags.auto_reload_on_alarm = false,
        };
        gptimer_set_alarm_action(timer, &alarm_config);
    }
    return true;
}

static bool IRAM_ATTR handle_timer_2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer relay 2, 2 stoped ");

    timer_2_callback_data_t *callback_data_timer_2 = (timer_2_callback_data_t *)user_data;
    callback_data_timer_2->current_timer_2--;
    ESP_EARLY_LOGI("Check flow", "%d", callback_data_timer_2->current_timer_2);
    tm1637_set_number(led2, callback_data_timer_2->current_timer_2);
    if (!callback_data_timer_2->current_timer_2)
    {
        restart_timer = true;
    }
    if (callback_data_timer_2->current_timer_2 == 1)
    {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = 60000000,
            .reload_count = 0,
            .flags.auto_reload_on_alarm = false,
        };
        gptimer_set_alarm_action(timer, &alarm_config);
    }
    return true;
}

static bool IRAM_ATTR handle_timer_3(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    timer_callback_data_t *callback_data = (timer_callback_data_t *)user_data;
    if (relay_3)
    {
        gpio_set_level(PIN_RELAY_3, 0);
        relay_3 = false;
        ESP_EARLY_LOGI("Check flow", "Relay 3 stopped (OFF)");
    }
    else
    {
        if (!relay_1)
        {
            gpio_set_level(PIN_RELAY_3, 1);
            relay_3 = true;
            ESP_EARLY_LOGI("Check flow", "Relay 3 started (ON)");
        }
    }
    relay_on = gpio_get_level(PIN_RELAY_3);
    tm1637_set_number(led3, relay_on ? callback_data->timer_3_duration_us_on / 60000000 : callback_data->timer_3_duration_us_off / 60000000);
    if (callback_data->free)
    {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = (relay_on ? callback_data->timer_3_duration_us_on : callback_data->timer_3_duration_us_off),
            .reload_count = 0,
            .flags.auto_reload_on_alarm = true,
        };
        gptimer_set_alarm_action(timer, &alarm_config);
    }
    return true;
}

void setup_timer_1(uint64_t timer_1_duration_us)
{
    ESP_LOGI(TAG, "Creating timer 1 handle");
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_1));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_1,
    };
    timer_1_callback_data_t *callback_data_timer_1 = malloc(sizeof(timer_1_callback_data_t));

    callback_data_timer_1->current_timer_1 = (int)timer_1_duration_us / 60000000;
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_1, &cbs, callback_data_timer_1));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_1));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_1_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 60000000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = timer_1_duration_us > 60000000,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_1, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer_1));
}
void setup_timer_2(uint64_t timer_2_duration_us)
{
    ESP_LOGI(TAG, "Creating timer 2 handle");
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_2));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_2,
    };
    timer_2_callback_data_t *callback_data_timer_2 = malloc(sizeof(timer_2_callback_data_t));

    callback_data_timer_2->current_timer_2 = (int)timer_2_duration_us / 60000000;

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_2, &cbs, callback_data_timer_2));

    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_2));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds", timer_2_duration_us);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 60000000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = timer_2_duration_us > 60000000,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_2, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer_2));
}
void setup_timer_3(uint64_t timer_3_duration_us_on, uint64_t timer_3_duration_us_off, bool free)
{
    ESP_LOGI(TAG, "Creating timer 3 handle");
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_3));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = handle_timer_3,
    };
    timer_callback_data_t *callback_data = malloc(sizeof(timer_callback_data_t));

    callback_data->free = free;
    callback_data->timer_3_duration_us_on = timer_3_duration_us_on;
    callback_data->timer_3_duration_us_off = timer_3_duration_us_off;

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_3, &cbs, callback_data));
    ESP_LOGI(TAG, "Enabling timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer_3));

    ESP_LOGI(TAG, "Starting timer for %llu microseconds (ON phase)", timer_3_duration_us_on);
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = timer_3_duration_us_on,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = free,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_3, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer_3));
}

void check_flag_timer(void *par)
{
    while (true)
    {

        if (flag_timer_2)
        {
            flag_timer_2 = false;
            gpio_set_level(PIN_RELAY_3, 1);
            vTaskDelay(pdMS_TO_TICKS(10000));
            gpio_set_level(PIN_RELAY_3, 0);
            gpio_set_level(PIN_RELAY_1, 1);
            relay_1 = true;
            setup_timer_2(timer_2 * 60000000);
        }
        if (restart_timer)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            esp_restart();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void init_data_timer(void)
{
    esp_err_t err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
    esp_err_t err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);
    esp_err_t err_timer_3 = nvs_open("timer_3", NVS_READONLY, &nvs_handle_timer_3);
    esp_err_t err_timer_4 = nvs_open("timer_4", NVS_READONLY, &nvs_handle_timer_4);
    err_timer_1 = nvs_get_u64(nvs_handle_timer_1, "TIMER_1_KEY", &timer_1);
    if (err_timer_1 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_1: %s", esp_err_to_name(err_timer_1));
    }

    err_timer_2 = nvs_get_u64(nvs_handle_timer_2, "TIMER_2_KEY", &timer_2);
    if (err_timer_2 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_2: %s", esp_err_to_name(err_timer_2));
    }
    err_timer_3 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", &timer_3_on);
    err_timer_3 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", &timer_3_off);

    if (err_timer_3 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_3: %s", esp_err_to_name(err_timer_3));
    }

    err_timer_4 = nvs_get_u64(nvs_handle_timer_4, "TIMER_4_KEY", &timer_4);
    if (err_timer_4 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_4: %s", esp_err_to_name(err_timer_4));
    }

    // Close the NVS handles
    nvs_close(nvs_handle_timer_1);
    nvs_close(nvs_handle_timer_2);
    nvs_close(nvs_handle_timer_3);
    nvs_close(nvs_handle_timer_4);
    printf("Timer 1: %llu\n", timer_1);
    printf("Timer 2: %llu\n", timer_2);
    printf("Timer 3 on: %llu\n", timer_3_on);
    printf("Timer 3 off: %llu\n", timer_3_off);
    printf("Timer 4: %llu\n", timer_4);
    // tm1637_lcd_t *led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    // tm1637_lcd_t *led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    // tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
}
