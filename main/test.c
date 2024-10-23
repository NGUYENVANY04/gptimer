// #include <stdio.h>
// #include <driver/gpio.h>
// #include "function_keys.h"
// #include "display_tm1637.h"
// #include "savedata.h"
// #include "handle_setting_timer_3.h"

// uint32_t last_isr_time_3 = 0;
// uint32_t last_countsetting_time_3 = 0;
// TaskHandle_t task_setting_3_on = NULL;
// TaskHandle_t increase_option_3 = NULL;
// TaskHandle_t decreased_option_3 = NULL;
// TaskHandle_t save_task_setting_3 = NULL;
// TaskHandle_t task_setting_3_off = NULL;

// uint64_t current_timer_3_on = 0;
// uint64_t current_timer_3_off = 0;

// uint32_t last_increase_isr_time_3 = 0;
// uint32_t last_decrease_isr_time_3 = 0;

// int count_setting_3 = 1;
// int current_step = 1; // 1 for setting "on" time, 2 for setting "off" time

// void init_data_3();
// void IRAM_ATTR setting_isr_handler_3_on(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();

//     if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(task_setting_3_on, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }
// void IRAM_ATTR setting_isr_handler_3_off(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();

//     if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(task_setting_3_off, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// void IRAM_ATTR setting_isr_save_3(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();
//     if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(save_task_setting_3, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// void IRAM_ATTR increase_option_isr_handler_3(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();
//     if (current_time - last_increase_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_increase_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(increase_option_3, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// void IRAM_ATTR decreased_option_isr_handler_3(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();
//     if (current_time - last_decrease_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_decrease_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//         vTaskNotifyGiveFromISR(decreased_option_3, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// void setting_3_on(void *arg)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for notification from ISR
//         uint32_t current_time = xTaskGetTickCountFromISR();
//         ESP_LOGI("Log", "Setting step: %d", current_step);

//         if (count_setting_3 == 1)
//         {
//             last_countsetting_time_3 = current_time;
//             count_setting_3 = 2;
//         }
//         else if (count_setting_3 == 2)
//         {
//             current_time = xTaskGetTickCountFromISR();

//             if (current_step == 1 && current_time - last_countsetting_time_3 < 2000 / portTICK_PERIOD_MS)
//             {
//                 ESP_LOGI("Option", "Enter ON timer setup");
//                 gpio_isr_handler_remove(15);
//                 gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3_off, NULL);
//                 gpio_isr_handler_add(GPIO_NUM_0, increase_option_isr_handler_3, NULL);
//                 gpio_isr_handler_add(GPIO_NUM_12, decreased_option_isr_handler_3, NULL);
//                 last_countsetting_time_3 = current_time;
//             }
//             else if (current_step == 2 && current_time - last_countsetting_time_3 < 2000 / portTICK_PERIOD_MS)
//             {
//                 ESP_LOGI("Option", "Enter OFF timer setup");
//                 gpio_isr_handler_remove(15);
//                 gpio_isr_handler_add(GPIO_NUM_15, setting_isr_save_3, NULL);
//                 gpio_isr_handler_add(GPIO_NUM_0, increase_option_isr_handler_3, NULL);
//                 gpio_isr_handler_add(GPIO_NUM_12, decreased_option_isr_handler_3, NULL);
//                 last_countsetting_time_3 = current_time;
//             }
//             count_setting_3 = 1;
//         }
//     }
// }

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
// void increase_3(void *arg)
// {
//     tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         if (current_step == 1) // Adjusting ON time
//         {
//             current_timer_3_on += 1;
//             tm1637_set_number(led3, current_timer_3_on);
//             ESP_LOGI(" Timer3 ", "Increased ON time: %lld ms", current_timer_3_on);
//         }
//         else if (current_step == 2) // Adjusting OFF time
//         {
//             current_timer_3_off += 1;
//             tm1637_set_number(led3, current_timer_3_off);
//             ESP_LOGI(" Timer3 ", "Increased OFF time: %lld ms", current_timer_3_off);
//         }
//     }
// }
// void decreased_3(void *arg)
// {
//     tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         if (current_step == 1) // Adjusting ON time
//         {
//             if (current_timer_3_on > 0)
//             {
//                 current_timer_3_on -= 1;
//             }
//             tm1637_set_number(led3, current_timer_3_on);
//             ESP_LOGI(" Timer3 ", "Decreased ON time: %lld ms", current_timer_3_on);
//         }
//         else if (current_step == 2) // Adjusting OFF time
//         {
//             if (current_timer_3_off > 0)
//             {
//                 current_timer_3_off -= 1;
//             }
//             tm1637_set_number(led3, current_timer_3_off);
//             ESP_LOGI(" Timer3 ", "Decreased OFF time: %lld ms", current_timer_3_off);
//         }
//     }
// }
// void save_data_3()
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
//         ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));
//         esp_err_t err_on = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", current_timer_3_on);
//         esp_err_t err_off = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", current_timer_3_off);
//         if (err_on != ESP_OK || err_off != ESP_OK)
//         {
//             ESP_LOGE("Save Error", "Failed to save timer data!");
//         }
//         else
//         {
//             ESP_LOGI("Save", "Timer data saved successfully!");
//         }
//         nvs_close(nvs_handle_timer_3);
//         init_data_3();
//         gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3_on, NULL);
//     }
// }

// void init_data_3()
// {
//     esp_err_t err_on = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", &current_timer_3_on);
//     esp_err_t err_off = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", &current_timer_3_off);
//     if (err_on != ESP_OK || err_off != ESP_OK)
//     {
//         ESP_LOGE("Read Error", "Failed to read timer data!");
//     }
//     else
//     {
//         ESP_LOGI("Init Data", "Initialized ON timer: %lld ms, OFF timer: %lld ms", current_timer_3_on, current_timer_3_off);
//     }
// }

// void setup_3_timer()
// {
//     // Initialize GPIO pins for setting, increase, and decrease
//     gpio_set_direction(GPIO_NUM_15, GPIO_MODE_INPUT);
//     gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT);
//     gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);

//     gpio_set_intr_type(GPIO_NUM_15, GPIO_INTR_NEGEDGE);
//     gpio_set_intr_type(GPIO_NUM_0, GPIO_INTR_NEGEDGE);
//     gpio_set_intr_type(GPIO_NUM_12, GPIO_INTR_NEGEDGE);

//     gpio_install_isr_service(0);
//     gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3_on, NULL);

//     xTaskCreate(setting_3_on, "setting_3_on", 2048, NULL, 5, &task_setting_3_on);
//     xTaskCreate(setting_3_off, "setting_3_off", 2048, NULL, 5, &task_setting_3_off);
//     xTaskCreate(increase_3, "increase_3", 2048, NULL, 5, &increase_option_3);
//     xTaskCreate(decreased_3, "decreased_3", 2048, NULL, 5, &decreased_option_3);
//     xTaskCreate(save_data_3, "save_data_3", 2048, NULL, 5, &save_task_setting_3);

//     init_data_3();
// }
