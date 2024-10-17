#include <stdio.h>
#include <driver/gpio.h>
#include "handle_isr.h"
#include "function_keys.h"
#include "display_tm1637.h"
#include "savedata.h"
uint32_t last_isr_time = 0;
uint32_t last_countsetting_time = 0;
TaskHandle_t task_setting = NULL;
TaskHandle_t toggle_option = NULL;
TaskHandle_t increase_option = NULL;
TaskHandle_t decreased_option = NULL;
TaskHandle_t save_task_setting = NULL;

uint64_t current_timer_1 = 0;
uint64_t current_timer_2 = 0;
uint32_t last_increase_isr_time = 0;
uint32_t last_decrease_isr_time = 0;

int count_setting = 1;
bool current_option = true;

void option_1();
void init_data_1();
void blink_led()
{
    ESP_LOGI("Option", "Interrupt detected, go option");
}
void IRAM_ATTR setting_isr_handler(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time = current_time;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_setting, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void IRAM_ATTR setting_isr_save(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time = current_time;

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(save_task_setting, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR increase_option_isr_handler(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_increase_isr_time > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_increase_isr_time = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(increase_option, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR decreased_option_isr_handler(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_decrease_isr_time > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_decrease_isr_time = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(decreased_option, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR option_isr_handler(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(toggle_option, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void setting(void *arg)
{
    while (true)
    {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification from ISR
        uint32_t current_time = xTaskGetTickCountFromISR();
        ESP_LOGI("Log", "%d", count_setting);
        if (count_setting == 1)
        {
            last_countsetting_time = current_time;
            count_setting = 2;
        }
        else if (count_setting == 2)
        {
            current_time = xTaskGetTickCountFromISR();

            if (current_time - last_countsetting_time < 2000 / portTICK_PERIOD_MS)
            {
                blink_led();
                gpio_isr_handler_remove(18);
                gpio_isr_handler_add(GPIO_NUM_18, option_isr_handler, NULL);
                gpio_isr_handler_add(GPIO_NUM_16, increase_option_isr_handler, NULL);
                gpio_isr_handler_add(GPIO_NUM_17, decreased_option_isr_handler, NULL);
                option_1();
                last_countsetting_time = current_time;
            }

            count_setting = 1;
        }
    }
}
void increase(void *arg)
{
    tm1637_lcd_t *led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    tm1637_lcd_t *led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current_option)
        {
            current_timer_1 += 1;
            ESP_LOGI("Option 1", "Increased time: %lld ms", current_timer_1);
            tm1637_set_number(led1, current_timer_1);
        }
        else
        {

            current_timer_2 += 1;
            ESP_LOGI("Option 2", "Increased time: %lld ms", current_timer_2);
            tm1637_set_number(led2, current_timer_2);
        }
    }
}

void decreased(void *arg)
{
    tm1637_lcd_t *led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    tm1637_lcd_t *led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current_option)
        {
            if (current_timer_1 >= 1)
            {
                current_timer_1 -= 1;
                ESP_LOGI("Option 1", "Decreased time: %lld ms", current_timer_1);
                tm1637_set_number(led1, current_timer_1);
            }
        }
        else
        {
            if (current_timer_2 >= 1)
            {
                current_timer_2 -= 1;
                ESP_LOGI("Option 2", "Decreased time: %lld ms", current_timer_2);
                tm1637_set_number(led2, current_timer_2);
            }
        }
    }
}
void save_data()
{
    while (true)
    {

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_ERROR_CHECK(nvs_open("timer_1", NVS_READWRITE, &nvs_handle_timer_1));
        esp_err_t err = nvs_set_u64(nvs_handle_timer_1, "TIMER_1_KEY", current_timer_1);
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_1));

        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error (%s) saving timer_1", esp_err_to_name(err));
        }
        else
        {
            uint64_t test = 0;
            err = nvs_get_u64(nvs_handle_timer_1, "TIMER_1_KEY", &test);
            if (err != ESP_OK)
            {
                ESP_LOGE("NVS", "Error (%s) geting timer_1", esp_err_to_name(err));
            }
            else
            {
                ESP_LOGI("test _1", "%d", (int)test);
            }
        }

        nvs_close(nvs_handle_timer_1);

        ESP_ERROR_CHECK(nvs_open("timer_2", NVS_READWRITE, &nvs_handle_timer_2));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_2, "TIMER_2_KEY", current_timer_2));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_2));
        nvs_close(nvs_handle_timer_2);
        ESP_LOGI("test", "%d", (int)current_timer_1);
        ESP_LOGI("test", "%d", (int)current_timer_2);
        esp_restart();
    }
}
void option_1()
{
    current_option = true;
    ESP_LOGI("Log", "Option 1");
    ESP_LOGI("Log", "Change timer Option 1");
}
void option_2()
{
    gpio_isr_handler_remove(18);
    gpio_isr_handler_add(GPIO_NUM_18, setting_isr_save, NULL);
    current_option = false;
    ESP_LOGI("Log", "Option 2");
    ESP_LOGI("Log", "Change timer Option 2");
}

void option(void *arg)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current_option)
        {
            option_2();
        }
        else
        {
            option_1();
        }
    }
}

void init_setting_timer_1_2(void)
{
    // led_t *led = {
    //     tm1637_init(LCD_CLK_1, LCD_DTA_1),
    //     tm1637_init(LCD_CLK_2, LCD_DTA_2),
    // };
    init_data_1();
    gpio_isr_handler_add(GPIO_NUM_18, setting_isr_handler, NULL);
    xTaskCreate(setting, "setting", 4096, NULL, 2, &task_setting);
    xTaskCreate(option, "option", 4096, NULL, 3, &toggle_option);
    xTaskCreate(increase, "increase", 4096, NULL, 4, &increase_option);
    xTaskCreate(decreased, "decreased", 4096, NULL, 5, &decreased_option);
    xTaskCreate(save_data, "save_data", 4096, NULL, 6, &save_task_setting);
}
// function provide data not start by 0
void init_data_1()
{
    // Open NVS for timer_1
    esp_err_t err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
    esp_err_t err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);

    // Read stored timer values
    err_timer_1 = nvs_get_u64(nvs_handle_timer_1, "TIMER_1_KEY", &current_timer_1);
    if (err_timer_1 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_1: %s", esp_err_to_name(err_timer_1));
    }

    err_timer_2 = nvs_get_u64(nvs_handle_timer_2, "TIMER_2_KEY", &current_timer_2);
    if (err_timer_2 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_2: %s", esp_err_to_name(err_timer_2));
    }

    // Close the NVS handles
    nvs_close(nvs_handle_timer_1);
    nvs_close(nvs_handle_timer_2);

    // Print the timers
    // printf("Timer 1: %llu\n", current_timer_1);
    // printf("Timer 2: %llu\n", current_timer_2);

    // Check the validity of the timers and set up if needed
}
