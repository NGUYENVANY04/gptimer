// #include <stdio.h>
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "nvs_flash.h"
// #include "nvs.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/timers.h"

// #define GPIO_NUM_15 15 // GPIO của nút 15
// #define DEBOUNCE_DELAY_MS 300 // Thời gian debounce cho nút nhấn

// // Định nghĩa các giá trị option
// bool current = true; // true = Option 1, false = Option 2

// // Biến thời gian và dữ liệu timer
// uint64_t current_timer_3_on = 0;
// uint64_t current_timer_3_off = 0;
// uint32_t last_isr_time_3 = 0;

// // Các handle task và timer
// TaskHandle_t increase_option_3 = NULL;
// TaskHandle_t decreased_option_3 = NULL;
// TaskHandle_t toggle = NULL;
// TaskHandle_t save_task_setting_3 = NULL;
// TimerHandle_t auto_save_timer;

// // Hàm lưu dữ liệu
// void save_data_3()
// {
//     ESP_LOGI("Save", "Saving timer data...");

//     nvs_handle nvs_handle_timer_3;
//     ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));

//     esp_err_t err_on = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", current_timer_3_on);
//     esp_err_t err_off = nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", current_timer_3_off);

//     if (err_on != ESP_OK || err_off != ESP_OK)
//     {
//         ESP_LOGE("Save Error", "Failed to save timer data!");
//     }
//     else
//     {
//         ESP_LOGI("Save", "Timer data saved successfully!");
//     }

//     ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
//     nvs_close(nvs_handle_timer_3);
// }

// // Hàm callback tự động lưu sau 5 giây không thao tác
// void auto_save_timer_callback(TimerHandle_t xTimer)
// {
//     // Gọi hàm lưu dữ liệu sau 5 giây không có thao tác
//     save_data_3();
//     ESP_LOGI("Auto Save", "Data saved automatically after 5 seconds of inactivity.");
// }

// // Hàm tăng giá trị cho option 1 hoặc 2
// void increase_3(void *arg)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         if (current)
//         {
//             current_timer_3_on += 1;
//             ESP_LOGI("Option 1", "Increased time: %lld ms", current_timer_3_on);
//             tm1637_set_number(led3, current_timer_3_on);
//         }
//         else
//         {
//             current_timer_3_off += 1;
//             ESP_LOGI("Option 2", "Increased time: %lld ms", current_timer_3_off);
//             tm1637_set_number(led3, current_timer_3_off);
//         }

//         // Reset timer sau mỗi thao tác tăng giảm
//         last_isr_time_3 = xTaskGetTickCount();
//         xTimerReset(auto_save_timer, 0);
//     }
// }

// // Hàm giảm giá trị cho option 1 hoặc 2
// void decreased_3(void *arg)
// {
//     while (true)
//     {
//         ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

//         if (current)
//         {
//             if (current_timer_3_on >= 1)
//             {
//                 current_timer_3_on -= 1;
//                 ESP_LOGI("Option 1", "Decreased time: %lld ms", current_timer_3_on);
//                 tm1637_set_number(led3, current_timer_3_on);
//             }
//         }
//         else
//         {
//             if (current_timer_3_off >= 1)
//             {
//                 current_timer_3_off -= 1;
//                 ESP_LOGI("Option 2", "Decreased time: %lld ms", current_timer_3_off);
//                 tm1637_set_number(led3, current_timer_3_off);
//             }
//         }

//         // Reset timer sau mỗi thao tác tăng giảm
//         last_isr_time_3 = xTaskGetTickCount();
//         xTimerReset(auto_save_timer, 0);
//     }
// }

// // Hàm xử lý ngắt cho nút 15 (Toggle giữa 2 option)
// void IRAM_ATTR setting_isr_handler_3_on(void *arg)
// {
//     uint32_t current_time = xTaskGetTickCountFromISR();

//     // Kiểm tra thời gian debounce
//     if (current_time - last_isr_time_3 > DEBOUNCE_DELAY_MS / portTICK_PERIOD_MS)
//     {
//         last_isr_time_3 = current_time;
//         BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//         // Toggle trạng thái current
//         current = !current;

//         // Cập nhật Option mới
//         if (current)
//         {
//             ESP_LOGI("Option", "Switched to Option 1");
//         }
//         else
//         {
//             ESP_LOGI("Option", "Switched to Option 2");
//         }

//         vTaskNotifyGiveFromISR(toggle, &xHigherPriorityTaskWoken);
//         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//     }
// }

// // Hàm khởi tạo dữ liệu ban đầu cho timer
// void init_data_3()
// {
//     // Lấy dữ liệu từ NVS
//     nvs_handle nvs_handle_timer_3;
//     ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));

//     esp_err_t err_on = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", &current_timer_3_on);
//     esp_err_t err_off = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", &current_timer_3_off);

//     if (err_on != ESP_OK || err_off != ESP_OK)
//     {
//         ESP_LOGW("Init", "Failed to load timer data, using default values.");
//         current_timer_3_on = 0;
//         current_timer_3_off = 0;
//     }
//     else
//     {
//         ESP_LOGI("Init", "Loaded timer data successfully.");
//     }

//     nvs_close(nvs_handle_timer_3);
// }

// // Hàm cấu hình hệ thống
// void setup_3_timer(void)
// {
//     init_data_3();
//     tm1637_set_number(led3, current_timer_3_on);

//     // Tạo timer tự động lưu sau 5 giây
//     auto_save_timer = xTimerCreate("AutoSaveTimer", pdMS_TO_TICKS(5000), pdFALSE, (void *)0, auto_save_timer_callback);

//     // Tạo task xử lý tăng giảm thời gian
//     xTaskCreate(increase_3, "increase", 4096, NULL, 4, &increase_option_3);
//     xTaskCreate(decreased_3, "decreased", 4096, NULL, 5, &decreased_option_3);
//     xTaskCreate(save_data_3, "save_data", 4096, NULL, 6, &save_task_setting_3);

//     // Cấu hình ngắt cho nút 15 (Toggle giữa 2 option)
//     gpio_isr_handler_add(GPIO_NUM_15, setting_isr_handler_3_on, NULL);
//     xTaskCreate(option_current, "option", 4096, NULL, 3, &toggle);
// }

// // Hàm chính
// void app_main(void)
// {
//     // Khởi tạo NVS
//     ESP_ERROR_CHECK(nvs_flash_init());

//     // Cấu hình hệ thống
//     setup_3_timer();
// }
