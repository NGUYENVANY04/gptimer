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

tm1637_lcd_t *led1, *led2, *led3;

void check_option(void *arg)
{
    while (true)
    {
        int state_xa_ep = gpio_get_level(GPIO_NUM_22);
        int state_xa_dau = gpio_get_level(GPIO_NUM_19);
        int state_xa_time = gpio_get_level(GPIO_NUM_23);
        int state_xa_temp = gpio_get_level(GPIO_NUM_34);
        ESP_LOGI("Check flow", "%d %d %d", state_xa_ep, state_xa_dau, state_xa_time);

        if (state_system)
        {
            if (state_xa_ep == true)
            {
                if (!option_a)
                {
                    ESP_LOGI("Check flow", "led 3 va relay 3 hoat dong doc lap voi he thong");
                    option_a = true;
                    gpio_set_level(PIN_RELAY_3, 1);
                    setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
                }
            }
            else if (state_xa_ep == false && state_xa_dau == false && state_xa_time == true)
            {
                if (!option_b)
                {
                    ESP_LOGI("Check flow", "hoat dong o che do binh thuong");
                    option_b = true;
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gpio_set_level(PIN_RELAY_2, 1);
                    vTaskDelay(pdMS_TO_TICKS(1800));
                    gpio_set_level(PIN_RELAY_2, 0);
                    gpio_set_level(PIN_RELAY_3, 1);
                    setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
                    setup_timer_1(timer_1 * 60000000);
                }
            }
            else if (state_xa_ep == false && state_xa_dau == true && state_xa_time == false)
            {
                if (!option_c)
                {
                    ESP_LOGI("Check flow", "hoat dong o che do nhiet do");
                    horizontal_row(led1);
                    option_c = true;
                    if (state_xa_temp == 1)
                    {
                        vTaskDelay(pdMS_TO_TICKS(10000));
                        gpio_set_level(PIN_RELAY_3, 1);
                        vTaskDelay(pdMS_TO_TICKS(10000));
                        gpio_set_level(PIN_RELAY_3, 0);
                        gpio_set_level(PIN_RELAY_2, 1);
                        setup_timer_2(timer_2 * 60000000);
                    }
                }
            }
            else
            {
                ESP_LOGI("Check flow", " No action");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void check_action_system(void *arg)
{
    while (true)
    {
        int system_action = gpio_get_level(GPIO_NUM_35);
        if (system_action && !state_system)
        {
            state_system = true;
            handle_data();
            init_setting_timer_1_2();
            setup_3_timer();
        }
        else if (!system_action && state_system)
        {
            esp_restart(); // Restart system if action is no longer active
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void app_main(void)
{
    led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
    clear_tm1637(led1);
    clear_tm1637(led2);
    clear_tm1637(led3);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    init_data_timer();

    congfi_io();
    xTaskCreate(check_flag_timer, "check timer flag", 4096, NULL, 10, NULL);
    xTaskCreate(check_action_system, " check_action_system ", 4096 / 2, NULL, 10, NULL);
    xTaskCreate(check_option, " check start system", 4096 / 2, NULL, 5, NULL);
}