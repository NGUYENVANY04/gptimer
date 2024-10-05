#include <stdio.h>
#include <driver/gpio.h>
#include "handle_isr.h"
#include "function_keys.h"
#include "display_tm1637.h"
uint32_t last_isr_time = 0;
uint32_t last_countsetting_time = 0;
TaskHandle_t task_setting = NULL;
TaskHandle_t toggle_option = NULL;
TaskHandle_t increase_option = NULL;
TaskHandle_t decreased_option = NULL;

uint64_t timer_1 = 0;
uint64_t timer_2 = 0;
uint32_t last_increase_isr_time = 0;
uint32_t last_decrease_isr_time = 0;

const gpio_num_t LCD_CLK_1 = 4;
const gpio_num_t LCD_DTA_1 = 5;
const gpio_num_t LCD_CLK_2 = 26;
const gpio_num_t LCD_DTA_2 = 25;

int count_setting = 1;
bool led_on = false; // State of the LED
bool current_option = true;
typedef struct
{
    tm1637_lcd_t *lcd1;
    tm1637_lcd_t *lcd2;

} led_t;
void option_1();
void blink_led()
{
    ESP_LOGI("Option", "Interrupt detected, go option");
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
                gpio_isr_handler_remove(16);
                gpio_isr_handler_add(GPIO_NUM_16, option_isr_handler, NULL);
                gpio_isr_handler_add(GPIO_NUM_18, increase_option_isr_handler, NULL);
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
            timer_1 += 1;
            ESP_LOGI("Option 1", "Increased time: %lld ms", timer_1);
            tm1637_set_number(led1, timer_1);
        }
        else
        {

            timer_2 += 1;

            ESP_LOGI("Option 2", "Increased time: %lld ms", timer_2);
            tm1637_set_number(led2, timer_2);
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
            if (timer_1 >= 1)
            {
                timer_1 -= 1;
                ESP_LOGI("Option 1", "Decreased time: %lld ms", timer_1);
                tm1637_set_number(led1, timer_1);
            }
        }
        else
        {
            if (timer_2 >= 1)
            {
                timer_2 -= 1;
                ESP_LOGI("Option 2", "Decreased time: %lld ms", timer_2);
                tm1637_set_number(led2, timer_2);
            }
        }
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
    // gpio_isr_handler_remove(16);
    // gpio_isr_handler_add(GPIO_NUM_16, setting_isr_handler, NULL);
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

void init_setting(void)
{
    // led_t *led = {
    //     tm1637_init(LCD_CLK_1, LCD_DTA_1),
    //     tm1637_init(LCD_CLK_2, LCD_DTA_2),
    // };
    congfi_io();
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_16, setting_isr_handler, NULL);
    xTaskCreate(setting, "setting", 4096, NULL, 2, &task_setting);
    xTaskCreate(option, "option", 4096, NULL, 3, &toggle_option);
    xTaskCreate(increase, "increase", 4096, NULL, 4, &increase_option);
    xTaskCreate(decreased, "decreased", 4096, NULL, 5, &decreased_option);
}
