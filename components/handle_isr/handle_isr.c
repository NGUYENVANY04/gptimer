#include <stdio.h>
#include "handle_isr.h"
#include "function_keys.h"
#define DEBOUNCE_DELAY_MS 200 // Define debounce delay

portMUX_TYPE button_mux = portMUX_INITIALIZER_UNLOCKED;
uint32_t last_isr_time = 0;
uint32_t last_countsetting_time = 0;


int count_setting;
bool led_on = false; // State of the LED
bool setting_val = false;
// ISR handler for button press on GPIO 15
void blink_led()
{
    ESP_LOGI("LED Task", "Interrupt detected, toggling LED...");

    led_on = !led_on;                   // Toggle the LED state
    gpio_set_level(GPIO_NUM_2, led_on); // Set the GPIO 2 (LED) to new state
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
        if (count_setting == 0)
        {
            last_countsetting_time = current_time;
            count_setting = 1;
        }
        else if (count_setting == 1)
        {
            current_time = xTaskGetTickCountFromISR();

            if (current_time - last_countsetting_time < 2000 / portTICK_PERIOD_MS)
            {
                blink_led();
                gpio_isr_handler_remove(15);
                gpio_isr_handler_add(GPIO_NUM_15, option_isr_handler, NULL);

                last_countsetting_time = current_time;
            }

            count_setting = 0;
        }
    }
}
void option(void *arg)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification from ISR
        ESP_LOGI("Log", "login option");
    }
}