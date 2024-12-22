#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
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
bool option_e = false;
bool state_xa_temp = false;
bool previous_state_xa_temp = true;
bool timer_3 = false;
bool timer_4_option_e = false;
bool timer_4_option_c = false;

bool start_up_state = false;
bool start_up_state_last = true;
bool option_e_reset = true;

tm1637_lcd_t *led1, *led2, *led3;
TaskHandle_t check_option_task_handle = NULL;
TaskHandle_t check_option_a = NULL;

bool relay_1 = false;
bool relay_3 = false;
bool delay_state = true;
bool led_state = true;
bool led_state_off = true;

void reset_relay()
{
    gpio_set_level(PIN_RELAY_1, 0);
    gpio_set_level(PIN_RELAY_2, 0);
    gpio_set_level(PIN_RELAY_3, 0);
    relay_1 = false;
}
void reset_task_handle()
{

    option_a = option_b = option_c = option_d = option_e = false;
    if (state_timer_1)
    {
        ESP_LOGI(TAG, "Stop gptimer_1");
        ESP_ERROR_CHECK(gptimer_stop(gptimer_1));
        ESP_LOGI(TAG, "Disable gptimer_1");
        ESP_ERROR_CHECK(gptimer_disable(gptimer_1));
        ESP_LOGI(TAG, "Delete gptimer_1");
        ESP_ERROR_CHECK(gptimer_del_timer(gptimer_1));
        state_timer_1 = false;
    }
    if (state_timer_2)
    {
        state_timer_2 = false;
        ESP_LOGI(TAG, "Stop gptimer_2");
        ESP_ERROR_CHECK(gptimer_stop(gptimer_2));
        ESP_LOGI(TAG, "Disable gptimer_2");
        ESP_ERROR_CHECK(gptimer_disable(gptimer_2));
        ESP_LOGI(TAG, "Delete gptimer_2");
        ESP_ERROR_CHECK(gptimer_del_timer(gptimer_2));
    }
    if (state_timer_3)
    {
        state_timer_3 = false;
        ESP_LOGI(TAG, "Stop gptimer_3");
        ESP_ERROR_CHECK(gptimer_stop(gptimer_3));
        ESP_LOGI(TAG, "Disable gptimer_3");
        ESP_ERROR_CHECK(gptimer_disable(gptimer_3));
        ESP_LOGI(TAG, "Delete gptimer_3");
        ESP_ERROR_CHECK(gptimer_del_timer(gptimer_3));
    }
    if (state_timer_4)
    {
        state_timer_4 = false;
        ESP_LOGI(TAG, "Stop gptimer_4");
        ESP_ERROR_CHECK(gptimer_stop(gptimer_4));
        ESP_LOGI(TAG, "Disable gptimer_4");
        ESP_ERROR_CHECK(gptimer_disable(gptimer_4));
        ESP_LOGI(TAG, "Delete gptimer_4");
        ESP_ERROR_CHECK(gptimer_del_timer(gptimer_4));
    }
    horizontal_row(led1);
    horizontal_row(led2);
    horizontal_row(led3);

    vTaskDelay(pdMS_TO_TICKS(1000));
    tm1637_set_number(led1, timer_1);
    tm1637_set_number(led2, timer_2);
    tm1637_set_number(led3, timer_3_on);
}
void start_up() {}
void check_option(void *arg)
{
    while (true)
    {
        gpio_set_level(GPIO_NUM_25, led_state);
        led_state = !led_state;
        if (!state_system)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int state_xa_ep = gpio_get_level(GPIO_NUM_23);
        int state_xa_dau = gpio_get_level(GPIO_NUM_19);
        int state_temp = gpio_get_level(GPIO_NUM_22);
        int input = gpio_get_level(GPIO_NUM_34);
        ESP_LOGI("Check flow", "%d %d %d %d", state_xa_ep, state_xa_dau, state_temp, input);

        if (state_xa_ep && !state_xa_dau)
        {
            if (!option_a)
            {

                reset_relay();
                reset_task_handle();
                clear_tm1637(led1);
                clear_tm1637(led2);

                ESP_LOGI("Check flow", "led 3 va relay 3 hoat dong doc lap voi he thong");
                option_a = true;
                gpio_set_level(PIN_RELAY_3, 1);
                relay_3 = true;
                setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
            }
        }
        else if (!state_xa_ep && state_xa_dau && !state_temp)
        {
            if (!option_b)
            {
                reset_task_handle();
                reset_relay();

                ESP_LOGI("Check flow", "hoat dong o che do binh thuong");
                option_b = true;
                if (start_up_state)
                {
                    start_up_state = false;
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gpio_set_level(PIN_RELAY_2, 1);
                    vTaskDelay(pdMS_TO_TICKS(1800));
                    gpio_set_level(PIN_RELAY_2, 0);
                }
                gpio_set_level(PIN_RELAY_3, 1);
                relay_3 = true;
                setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, false);
                setup_timer_1(timer_1 * 60000000);
            }
        }
        else if (state_xa_ep && state_xa_dau && !state_temp)
        {
            if (!option_d)
            {
                reset_task_handle();
                reset_relay();
                ESP_LOGI("Check flow", "hoat dong o che do khong dung timer 3 va thoi gian");
                clear_tm1637(led3);
                option_d = true;
                if (start_up_state)
                {
                    start_up_state = false;
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gpio_set_level(PIN_RELAY_2, 1);
                    vTaskDelay(pdMS_TO_TICKS(1800));
                    gpio_set_level(PIN_RELAY_2, 0);
                }
                setup_timer_1(timer_1 * 60000000);
            }
        }
        else if (!state_xa_ep && state_xa_dau && state_temp)
        {
            if (!option_c)
            {
                ESP_LOGI("Check flow", "hoat dong o che do xa dau & nhiet do");
                horizontal_row(led1);
                state_xa_temp = gpio_get_level(GPIO_NUM_34);

                if (start_up_state)
                {
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gpio_set_level(PIN_RELAY_2, 1);
                    vTaskDelay(pdMS_TO_TICKS(1800));
                    gpio_set_level(PIN_RELAY_2, 0);
                    start_up_state = false;
                }

                if (!timer_3)
                {
                    reset_task_handle();
                    horizontal_row(led1);
                    reset_relay();
                    timer_3 = true;
                    gpio_set_level(PIN_RELAY_3, 1);
                    relay_3 = true;
                    setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
                }
                if (!timer_4_option_c)
                {
                    setup_timer_4(15 * 60000000);
                    timer_4_option_c = true;
                }
                if (state_xa_temp && !previous_state_xa_temp && !relay_3)
                {
                    //----------------------------
                    ESP_LOGI(TAG, "Stop gptimer_4");
                    ESP_ERROR_CHECK(gptimer_stop(gptimer_4));
                    ESP_LOGI(TAG, "Disable gptimer_4");
                    ESP_ERROR_CHECK(gptimer_disable(gptimer_4));
                    ESP_LOGI(TAG, "Delete gptimer_4");
                    ESP_ERROR_CHECK(gptimer_del_timer(gptimer_4));
                    state_timer_4 = false;
                    //----------------------------
                    option_c = true;
                    ESP_LOGI("Check flow", "hoat dong o che do nhiet do");
                    gpio_set_level(PIN_RELAY_3, 1);
                    vTaskDelay(pdMS_TO_TICKS(10000));
                    gpio_set_level(PIN_RELAY_3, 0);
                    gpio_set_level(PIN_RELAY_1, 1);
                    relay_1 = true;
                    setup_timer_2(timer_2 * 60000000);
                }
                previous_state_xa_temp = state_xa_temp;
            }
        }

        else if (state_xa_ep && state_xa_dau && state_temp)
        {
            if (!option_e)
            {
                ESP_LOGI("Check flow", "hoat dong o che do khong dung timer 3 va nhiet do");
                state_xa_temp = gpio_get_level(GPIO_NUM_34);

                if (option_e_reset)
                {
                    reset_task_handle();
                    clear_tm1637(led3);
                    horizontal_row(led1);
                    reset_relay();
                    option_e_reset = false;
                }
                if (!timer_4_option_e)
                {
                    setup_timer_4(15 * 60000000);
                    timer_4_option_e = true;
                }

                if (start_up_state)
                {
                    start_up_state = false;
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    gpio_set_level(PIN_RELAY_2, 1);
                    vTaskDelay(pdMS_TO_TICKS(1800));
                    gpio_set_level(PIN_RELAY_2, 0);
                }
                if (state_xa_temp && !previous_state_xa_temp)
                {
                    //----------------------------
                    ESP_LOGI(TAG, "Stop gptimer_4");
                    ESP_ERROR_CHECK(gptimer_stop(gptimer_4));
                    ESP_LOGI(TAG, "Disable gptimer_4");
                    ESP_ERROR_CHECK(gptimer_disable(gptimer_4));
                    ESP_LOGI(TAG, "Delete gptimer_4");
                    ESP_ERROR_CHECK(gptimer_del_timer(gptimer_4));
                    state_timer_4 = false;
                    //----------------------------
                    option_e = true;
                    ESP_LOGI("Check flow", "hoat dong o che do nhiet do");
                    gpio_set_level(PIN_RELAY_3, 1);
                    vTaskDelay(pdMS_TO_TICKS(10000));
                    gpio_set_level(PIN_RELAY_3, 0);
                    gpio_set_level(PIN_RELAY_1, 1);
                    relay_1 = true;
                    setup_timer_2(timer_2 * 60000000);
                }

                previous_state_xa_temp = state_xa_temp;
            }
        }
        else
        {
            ESP_LOGI("Check flow", " No action");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void option_doclap(void *arg);
void check_action_system(void *arg)
{
    while (true)
    {

        // ESP_LOGI("Check flow", "%s", state_system ? "Run" : "Stop");
        int system_action = gpio_get_level(GPIO_NUM_35);
        // if (system_action && !start_up_state_last)
        // {
        //     start_up_state = true;
        // }
        // start_up_state_last = system_action;

        if (system_action && !state_system)
        {
            gpio_set_level(GPIO_NUM_26, !led_state_off);
            state_system = true;
            handle_data();
            init_setting_timer_1_2();
            setup_3_timer();
            tm1637_set_number(led1, timer_1);
            tm1637_set_number(led2, timer_2);
            tm1637_set_number(led3, timer_3_on);

            if (status)
            {
                start_up_state = true;
            }
            ESP_LOGI("Check flow", "Check state %d", start_up_state_last);
            ESP_LOGI("Check flow", "Check state %d", start_up_state);

            // ----------------------------------------------
            ESP_ERROR_CHECK(nvs_open("startup", NVS_READWRITE, &startup_handle));
            ESP_ERROR_CHECK(nvs_set_i8(startup_handle, "status", 0));
            ESP_ERROR_CHECK(nvs_commit(startup_handle));
            nvs_close(startup_handle);
            ESP_LOGI("State ", "%d", (int)status);
            // ----------------------------------------------

            if (check_option_task_handle == NULL)
            {
                xTaskCreate(check_option, "check start system", 4096 / 2, NULL, 5, &check_option_task_handle);
            }
        }
        else if (!system_action && state_system)
        {
            gpio_set_level(GPIO_NUM_26, led_state_off);
            if (check_option_a == NULL)
            {
                ESP_LOGI("Check option_doclap", "abc");
                xTaskCreate(option_doclap, "check start option doc lap", 4096 / 2, NULL, 5, &check_option_a);
            }
            state_system = false;
            option_a = option_b = option_c = option_d = option_e = false;
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
        if (!system_action)
        {
            start_up_state_last = false;
            ESP_LOGI("Check flow", "Check state %d", start_up_state_last);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void check_relay1(void *arg)
{
    while (true)
    {
        if (relay_1)
        {
            if (relay_3)
            {
                gpio_set_level(PIN_RELAY_3, 0);
                relay_3 = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void option_doclap(void *arg)
{
    while (true)
    {
        int state_xa_ep = gpio_get_level(GPIO_NUM_23);
        int state_xa_dau = gpio_get_level(GPIO_NUM_19);
        gpio_set_level(GPIO_NUM_25, led_state);
        led_state = !led_state;
        if (state_xa_ep && !state_xa_dau)
        {
            if (!option_a)
            {
                clear_tm1637(led1);
                clear_tm1637(led2);
                tm1637_set_number(led3, timer_3_on);
                ESP_LOGI("Check flow", "led 3 va relay 3 hoat dong doc lap voi he thong (TH2)");
                option_a = true;
                gpio_set_level(PIN_RELAY_3, 1);
                relay_3 = true;
                setup_timer_3(timer_3_on * 60000000, timer_3_off * 60000000, true);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void app_main(void)
{
    led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    // horizontal_row(led1);
    // horizontal_row(led2);
    // horizontal_row(led3);

    vTaskDelay(pdMS_TO_TICKS(1000));

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
    xTaskCreate(check_relay1, " check_relay1 ", 4096 / 2, NULL, 5, NULL);
}
