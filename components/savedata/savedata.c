#include "savedata.h"
#include "timer_handle.h"
#include "esp_log.h"
#include "display_tm1637.h"
#include "function_keys.h"
uint64_t timer_1 = 0, timer_2 = 0;
nvs_handle nvs_handle_timer_1;
nvs_handle nvs_handle_timer_2;

void handle_data()
{
    tm1637_lcd_t *led1 = tm1637_init(4, 5);
    tm1637_lcd_t *led2 = tm1637_init(26, 25);
    // Open NVS for timer_1
    esp_err_t err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
    esp_err_t err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);

    if (err_timer_1 == ESP_ERR_NVS_NOT_FOUND || err_timer_2 == ESP_ERR_NVS_NOT_FOUND)
    {
        printf("cos looix ");

        // Namespace does not exist, create it with default values
        ESP_ERROR_CHECK(nvs_open("timer_1", NVS_READWRITE, &nvs_handle_timer_1));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_1, "TIMER_1_KEY", timer_1));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_1));
        nvs_close(nvs_handle_timer_1);

        ESP_ERROR_CHECK(nvs_open("timer_2", NVS_READWRITE, &nvs_handle_timer_2));
        esp_err_t err = nvs_set_u64(nvs_handle_timer_2, "TIMER_2_KEY", timer_2);
        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error saving timer_2: %s", esp_err_to_name(err));
        }
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_2));
        nvs_close(nvs_handle_timer_2);

        // Re-open the namespace in read-only mode
        err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
        err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);
    }

    // Read stored timer values
    err_timer_1 = nvs_get_u64(nvs_handle_timer_1, "TIMER_1_KEY", &timer_1);
    if (err_timer_1 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_1: %s", esp_err_to_name(err_timer_1));
    }

    err_timer_2 = nvs_get_u64(nvs_handle_timer_2, "TIMER_2_KEY", &timer_2);
    if (err_timer_2 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_2: %s", esp_err_to_name(err_timer_2));
    }

    // Close the NVS handles
    nvs_close(nvs_handle_timer_1);
    nvs_close(nvs_handle_timer_2);

    // Print the timers
    printf("Timer 1: %llu\n", timer_1);
    printf("Timer 2: %llu\n", timer_2);

    // Check the validity of the timers and set up if needed
    if (err_timer_1 == ESP_OK && err_timer_2 == ESP_OK && timer_1 > 0 && timer_2 > 0)
    {
        setup_timer_1(timer_1 * 1000000);
        setup_timer_2(timer_2 * 1000000);
    }
    else
    {
        printf("No valid timers found.\n");
    }
    tm1637_set_number(led1, timer_1);
    tm1637_set_number(led2, timer_2);
}
