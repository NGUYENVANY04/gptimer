#include <stdio.h>
#include <driver/gpio.h>
#include "function_keys.h"
#include "display_tm1637.h"
#include "savedata.h"
#include "handle_setting_timer_3.h"
uint32_t last_isr_time_3 = 0;
uint32_t last_countsetting_time_3 = 0;
TaskHandle_t task_setting_3_on = NULL;
TaskHandle_t increase_option_3 = NULL;
TaskHandle_t decreased_option_3 = NULL;
TaskHandle_t save_task_setting_3 = NULL;
TaskHandle_t task_setting_3_off = NULL;
TaskHandle_t toggle = NULL;

uint64_t current_timer_3_on = 0;
uint64_t current_timer_3_off = 0;

uint32_t last_increase_isr_time_3 = 0;
uint32_t last_decrease_isr_time_3 = 0;
bool current = false;
int count_setting_3 = 1;

void init_data_3();
void IRAM_ATTR option_isr_handler_3(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(toggle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void IRAM_ATTR setting_isr_handler_3_on(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_setting_3_on, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
void IRAM_ATTR setting_isr_handler_3_off(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_setting_3_off, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR setting_isr_save_3(void *arg)
{

    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(save_task_setting_3, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR increase_option_isr_handler_3(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_increase_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_increase_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(increase_option_3, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void IRAM_ATTR decreased_option_isr_handler_3(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();
    if (current_time - last_decrease_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_decrease_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(decreased_option_3, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void setting_3_on(void *arg)
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification from ISR
        uint32_t current_time = xTaskGetTickCountFromISR();
        ESP_LOGI("Log", "%d", count_setting_3);
        if (count_setting_3 == 1)
        {
            last_countsetting_time_3 = current_time;
            count_setting_3 = 2;
        }
        else if (count_setting_3 == 2)
        {
            current_time = xTaskGetTickCountFromISR();

            if (current_time - last_countsetting_time_3 < 2000 / portTICK_PERIOD_MS)
            {
                tm1637_display_on(led3);
                ESP_LOGI("Option", "Interrupt detected, go option");
                gpio_isr_handler_remove(15);
                gpio_isr_handler_add(GPIO_NUM_15, option_isr_handler_3, NULL);
                gpio_isr_handler_add(GPIO_NUM_0, increase_option_isr_handler_3, NULL);
                gpio_isr_handler_add(GPIO_NUM_12, decreased_option_isr_handler_3, NULL);
                last_countsetting_time_3 = current_time;
                current = true;
            }
            count_setting_3 = 1;
        }
    }
}
void option_on()
{
    current = true;
    ESP_LOGI("Log", "Option 1");
    ESP_LOGI("Log", "Change timer on ");
}
void option_off()
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
    tm1637_display_off(led3);
    gpio_isr_handler_remove(15);
    gpio_isr_handler_add(GPIO_NUM_15, setting_isr_save_3, NULL);
    current = false;
    ESP_LOGI("Log", "Option 2");
    ESP_LOGI("Log", "Change timer Off");
}

void option_current(void *arg)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current)
        {
            option_off();
        }
        else
        {
            option_on();
        }
    }
}

// void setting_3_off(void *arg)
// {
//     tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         tm1637_display_off(led3);
//         gpio_isr_handler_remove(15);
//         gpio_isr_handler_add(GPIO_NUM_15, setting_isr_save_3, NULL);
//     }
// }
void increase_3(void *arg)
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current)
        {
            current_timer_3_on += 1;
            ESP_LOGI("Option 1", "Increased time: %lld ms", current_timer_3_on);
            tm1637_set_number(led3, current_timer_3_on);
        }
        else
        {

            current_timer_3_off += 1;
            ESP_LOGI("Option 2", "Increased time: %lld ms", current_timer_3_off);
            tm1637_set_number(led3, current_timer_3_off);
        }
    }
}
void decreased_3(void *arg)
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current)
        {
            if (current_timer_3_on >= 1)
            {
                current_timer_3_on -= 1;
                ESP_LOGI("Option 1", "Decreased time: %lld ms", current_timer_3_on);
                tm1637_set_number(led3, current_timer_3_on);
            }
        }
        else
        {
            if (current_timer_3_off >= 1)
            {
                current_timer_3_off -= 1;
                ESP_LOGI("Option 2", "Decreased time: %lld ms", current_timer_3_off);
                tm1637_set_number(led3, current_timer_3_off);
            }
        }
    }
}
void save_data_3()
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));
        esp_err_t err_on = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", current_timer_3_on);
        esp_err_t err_off = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", current_timer_3_off);
        if (err_on != ESP_OK || err_off != ESP_OK)
        {
            ESP_LOGE("Save Error", "Failed to save timer data!");
        }
        else
        {
            ESP_LOGI("Save", "Timer data saved successfully!");
        }
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
        nvs_close(nvs_handle_timer_3);
        esp_restart();
    }
}
void setup_3_timer(void)
{
    init_data_3();
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);
    tm1637_set_number(led3, current_timer_3_on);

    gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3_on, NULL);
    xTaskCreate(setting_3_on, "setting", 4096, NULL, 2, &task_setting_3_on);
    xTaskCreate(increase_3, "increase", 4096, NULL, 4, &increase_option_3);
    xTaskCreate(decreased_3, "decreased", 4096, NULL, 5, &decreased_option_3);
    xTaskCreate(save_data_3, "save_data", 4096, NULL, 6, &save_task_setting_3);
    xTaskCreate(option_current, "option", 4096, NULL, 3, &toggle);
}
void init_data_3()
{
    current_timer_3_off = timer_3_off;
    current_timer_3_on = timer_3_on;
}
