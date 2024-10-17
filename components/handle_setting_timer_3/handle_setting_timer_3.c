#include <stdio.h>
#include <driver/gpio.h>
#include "function_keys.h"
#include "display_tm1637.h"
#include "savedata.h"
#include "handle_setting_timer_3.h"
uint32_t last_isr_time_3 = 0;
uint32_t last_countsetting_time_3 = 0;
TaskHandle_t task_setting_3 = NULL;
TaskHandle_t increase_option_3 = NULL;
TaskHandle_t decreased_option_3 = NULL;
TaskHandle_t save_task_setting_3 = NULL;

uint64_t current_timer_3 = 0;
uint32_t last_increase_isr_time_3 = 0;
uint32_t last_decrease_isr_time_3 = 0;

int count_setting_3 = 1;

void init_data_3();
void IRAM_ATTR setting_isr_handler_3(void *arg)
{
    uint32_t current_time = xTaskGetTickCountFromISR();

    if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
    {
        last_isr_time_3 = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(task_setting_3, &xHigherPriorityTaskWoken);
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

void setting_3(void *arg)
{
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
                ESP_LOGI("Option", "Interrupt detected, go option");
                gpio_isr_handler_remove(15);
                gpio_isr_handler_add(GPIO_NUM_15, setting_isr_save_3, NULL);
                gpio_isr_handler_add(GPIO_NUM_0, increase_option_isr_handler_3, NULL);
                gpio_isr_handler_add(GPIO_NUM_12, decreased_option_isr_handler_3, NULL);
                last_countsetting_time_3 = current_time;
            }
            count_setting_3 = 1;
        }
    }
}
void increase_3(void *arg)
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        current_timer_3 += 1;
        tm1637_set_number(led3, current_timer_3);

        ESP_LOGI(" Timer3 ", "Increased time: %lld ms", current_timer_3);
    }
}

void decreased_3(void *arg)
{
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (current_timer_3 > 0)
        {
            current_timer_3 -= 1;
        }
        tm1637_set_number(led3, current_timer_3);

        ESP_LOGI(" Timer3 ", "Deincreased time: %lld ms", current_timer_3);
    }
}
void save_data_3()
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));
        esp_err_t err = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY", current_timer_3);
        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error (%s) saving timer_3", esp_err_to_name(err));
        }
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
        nvs_close(nvs_handle_timer_3);
        esp_restart();
    }
}
void init_setting_timer_3(void)
{
    init_data_3();
    gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3, NULL);
    xTaskCreate(setting_3, "setting", 4096, NULL, 2, &task_setting_3);
    xTaskCreate(increase_3, "increase", 4096, NULL, 4, &increase_option_3);
    xTaskCreate(decreased_3, "decreased", 4096, NULL, 5, &decreased_option_3);
    xTaskCreate(save_data_3, "save_data", 4096, NULL, 6, &save_task_setting_3);
}
void init_data_3()
{
    esp_err_t err_timer_3 = nvs_open("timer_3", NVS_READONLY, &nvs_handle_timer_3);
    err_timer_3 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY", &current_timer_3);
    if (err_timer_3 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_3: %s", esp_err_to_name(current_timer_3));
    }
    // printf("Timer 3: %llu\n", current_timer_3);
    nvs_close(nvs_handle_timer_3);
}
