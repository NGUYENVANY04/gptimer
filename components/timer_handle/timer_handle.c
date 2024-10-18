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
bool flag_timer_1 = true;
bool flag_timer_2 = false;
bool flag_timer_3 = false;
bool restart_timer = false;
// uint64_t timer_1 = 0;
// uint64_t timer_2 = 0;
// uint64_t timer_3 = 0;
// uint64_t timer_4 = 0;

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
    gptimer_del_timer(timer_1);
    return true;
}

static bool IRAM_ATTR handle_timer_2(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer  3 started , relay 2 continiuuu, 3 started ");
    gpio_set_level(PIN_RELAY_3, 1);
    flag_timer_3 = true;
    gptimer_del_timer(timer_2);

    return true;
}
static bool IRAM_ATTR handle_timer_3(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer 3 stoped , relay 1,3  stoped");
    flag_timer_1 = true;
    gptimer_del_timer(timer_3);

    gpio_set_level(PIN_RELAY_2, 0);
    gpio_set_level(PIN_RELAY_3, 0);

    return true;
}
static bool IRAM_ATTR handle_timer_4(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI("Check flow", "Timer 4 stoped , relay 2  stoped");
    gptimer_del_timer(timer_4);

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
            setup_timer_1(timer_1 * 60000000);
            ESP_ERROR_CHECK(gptimer_start(gptimer_1));
        }
        if (flag_timer_2)
        {
            flag_timer_2 = false;
            ESP_LOGI(TAG, "Starting timer 2 and timer 4");
            setup_timer_2(timer_2 * 60000000);
            setup_timer_4(timer_4 * 1000000);
            ESP_ERROR_CHECK(gptimer_start(gptimer_2));
            ESP_ERROR_CHECK(gptimer_start(gptimer_4));
        }
        if (flag_timer_3)
        {
            flag_timer_3 = false;
            setup_timer_3(timer_3 * 60000000);
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
void init_data_timer(void)
{

    esp_err_t err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
    esp_err_t err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);
    esp_err_t err_timer_3 = nvs_open("timer_3", NVS_READONLY, &nvs_handle_timer_3);
    esp_err_t err_timer_4 = nvs_open("timer_4", NVS_READONLY, &nvs_handle_timer_4);
    // Read stored timer values
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
    err_timer_3 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY", &timer_3);
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
    printf("Timer 3: %llu\n", timer_3);
    printf("Timer 4: %llu\n", timer_4);
    tm1637_lcd_t *led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    tm1637_lcd_t *led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    tm1637_set_number(led1, timer_1);
    tm1637_set_number(led2, timer_2);
    tm1637_set_number(led3, timer_3);
}