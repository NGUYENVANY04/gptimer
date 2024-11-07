#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include <sys/time.h>
#include <esp_system.h>
#include "handle_isr.h"
#include "display_tm1637.h"
#include "function_keys.h"
#include "savedata.h"
#include "webserver.h"
#include "handle_setting_timer_3.h"
#include "timer_handle.h"
#define TAG "app"

bool state_system = false;
bool start_xa_ep = false;
bool option_a = false;
bool option_b = false;
bool option_c = false;
bool option_d = false;
bool state_xa_temp = false;
bool previous_state_xa_temp = true;

tm1637_lcd_t *led1, *led2, *led3;
TaskHandle_t check_option_task_handle = NULL;

void check_option(void *arg)
{
    while (true)
    {
        if (!state_system)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        int state_xa_ep = gpio_get_level(GPIO_NUM_23); // xuong la xa ep
        int state_xa_dau = gpio_get_level(GPIO_NUM_19);
        int state_temp = gpio_get_level(GPIO_NUM_22); // len la nhiet do, xuong la time
        int input = gpio_get_level(GPIO_NUM_34);
        ESP_LOGI("Check flow", "%d %d %d %d", state_xa_ep, state_xa_dau, state_temp, input);

        if (state_xa_ep && !state_xa_dau)
        {
            if (!option_a)
            {
                clear_tm1637(led1);
                clear_tm1637(led2);
                ESP_LOGI("Check flow", "led 3 va relay 3 hoat dong doc lap voi he thong");
                option_a = option_b = option_c = option_d = true;
                gpio_set_level(PIN_RELAY_3, 1);
                setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
            }
        }
        else if (!state_xa_ep && state_xa_dau && !state_temp)
        {
            if (!option_b)
            {
                ESP_LOGI("Check flow", "hoat dong o che do binh thuong");
                option_a = option_b = option_c = option_d = true;
                vTaskDelay(pdMS_TO_TICKS(5000));
                gpio_set_level(PIN_RELAY_2, 1);
                vTaskDelay(pdMS_TO_TICKS(1800));
                gpio_set_level(PIN_RELAY_2, 0);
                gpio_set_level(PIN_RELAY_3, 1);
                setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
                setup_timer_1(timer_1 * 60000000);
            }
        }
        else if (!state_xa_ep && state_xa_dau && state_temp)
        {
            if (!option_c)
            {
                horizontal_row(led1);
                option_a = option_b = option_d = true;
                state_xa_temp = gpio_get_level(GPIO_NUM_34);
                if (state_xa_temp && !previous_state_xa_temp)
                {
                    option_c = true;
                    ESP_LOGI("Check flow", "hoat dong o che do nhiet do");
                    gpio_set_level(PIN_RELAY_1, 1);
                    setup_timer_2(timer_2 * 60000000);
                }
                previous_state_xa_temp = state_xa_temp;
            }
        }
        else if (state_xa_ep && state_xa_dau)
        {
            if (!option_d)
            {
                ESP_LOGI("Check flow", "hoat dong o che do khong dung timer 3");
                clear_tm1637(led3);
                option_a = option_b = option_c = option_d = true;
                vTaskDelay(pdMS_TO_TICKS(5000));
                gpio_set_level(PIN_RELAY_2, 1);
                vTaskDelay(pdMS_TO_TICKS(1800));
                gpio_set_level(PIN_RELAY_2, 0);
                setup_timer_1(timer_1 * 60000000);
            }
        }
        else
        {
            ESP_LOGI("Check flow", " No action");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void check_action_system(void *arg)
{
    while (true)
    {

        // ESP_LOGI("Check flow", "%s", state_system ? "Run" : "Stop");
        int system_action = gpio_get_level(GPIO_NUM_35);
        if (system_action && !state_system)
        {
            state_system = true;
            handle_data();
            init_setting_timer_1_2();
            setup_3_timer();
            tm1637_set_number(led1, timer_1);
            tm1637_set_number(led2, timer_2);
            tm1637_set_number(led3, timer_3_on);
            if (check_option_task_handle == NULL)
            {
                xTaskCreate(check_option, "check start system", 4096 / 2, NULL, 5, &check_option_task_handle);
            }
        }
        else if (!system_action && state_system)
        {
            state_system = false;
            option_a = option_b = option_c = option_d = false;
            if (check_option_task_handle != NULL)
            {
                vTaskDelete(check_option_task_handle);
                check_option_task_handle = NULL;
            }
            clear_tm1637(led1);
            clear_tm1637(led2);
            clear_tm1637(led3);
            esp_restart();
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
void app_main(void)
{
    led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
    // clear_tm1637(led1);
    // clear_tm1637(led2);
    // clear_tm1637(led3);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    congfi_io();
    init_data_timer();
    xTaskCreate(check_flag_timer, "check timer flag", 4096, NULL, 10, NULL);
    xTaskCreate(check_action_system, " check_action_system ", 4096 / 2, NULL, 10, NULL);
}