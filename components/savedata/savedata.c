#include "savedata.h"
#include "timer_handle.h"
#include "esp_log.h"
#include "display_tm1637.h"
#include "function_keys.h"
uint64_t timer_1 = 0, timer_2 = 0, timer_3 = 0, timer_4 = 0;
nvs_handle nvs_handle_timer_1;
nvs_handle nvs_handle_timer_2;
nvs_handle nvs_handle_timer_3;
nvs_handle nvs_handle_timer_4;

void handle_data()
{
    tm1637_lcd_t *led1 = tm1637_init(LCD_CLK_1, LCD_DTA_1);
    tm1637_lcd_t *led2 = tm1637_init(LCD_CLK_2, LCD_DTA_2);
    tm1637_lcd_t *led3 = tm1637_init(LCD_CLK_3, LCD_DTA_3);

    esp_err_t err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
    esp_err_t err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);
    esp_err_t err_timer_3 = nvs_open("timer_3", NVS_READONLY, &nvs_handle_timer_3);
    esp_err_t err_timer_4 = nvs_open("timer_4", NVS_READONLY, &nvs_handle_timer_4);
    if (err_timer_1 == ESP_ERR_NVS_NOT_FOUND || err_timer_2 == ESP_ERR_NVS_NOT_FOUND ||
        err_timer_3 == ESP_ERR_NVS_NOT_FOUND || err_timer_4 == ESP_ERR_NVS_NOT_FOUND)
    {

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

        ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY", timer_3));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
        nvs_close(nvs_handle_timer_3);

        ESP_ERROR_CHECK(nvs_open("timer_4", NVS_READWRITE, &nvs_handle_timer_4));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_4, "TIMER_4_KEY", timer_4));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_4));
        nvs_close(nvs_handle_timer_4);

        // Re-open the namespace in read-only mode
        err_timer_1 = nvs_open("timer_1", NVS_READONLY, &nvs_handle_timer_1);
        err_timer_2 = nvs_open("timer_2", NVS_READONLY, &nvs_handle_timer_2);
        err_timer_3 = nvs_open("timer_3", NVS_READONLY, &nvs_handle_timer_3);
        err_timer_4 = nvs_open("timer_4", NVS_READONLY, &nvs_handle_timer_4);
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
    err_timer_3 = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY", &timer_3);
    if (err_timer_3 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_3: %s", esp_err_to_name(err_timer_3));
    }

    err_timer_4 = nvs_get_u64(nvs_handle_timer_4, "TIMER_4_KEY", &timer_4);
    if (err_timer_4 != ESP_OK)
    {
        ESP_LOGE("NVS", "Error reading timer_4: %s", esp_err_to_name(err_timer_4));
    }

    // Close the NVS handles
    nvs_close(nvs_handle_timer_1);
    nvs_close(nvs_handle_timer_2);
    nvs_close(nvs_handle_timer_3);
    nvs_close(nvs_handle_timer_4);

    // Print the timers
    printf("Timer 1: %llu\n", timer_1);
    printf("Timer 2: %llu\n", timer_2);
    printf("Timer 3: %llu\n", timer_3);
    printf("Timer 4: %llu\n", timer_4);

    // Check the validity of the timers and set up if needed
    if (err_timer_1 == ESP_OK && err_timer_2 == ESP_OK &&
        timer_1 > 0 && timer_2 > 0 && timer_3 > 0 && timer_4 > 0)
    {
        setup_timer_1(timer_1 * 60000000);
        setup_timer_2(timer_2 * 60000000);
        setup_timer_3(timer_3 * 60000000);
        setup_timer_4(timer_4 * 1000000);
        }
    else
    {
        printf("No valid timers found.\n");
    }
    tm1637_set_number(led1, timer_1);
    tm1637_set_number(led2, timer_2);
    tm1637_set_number(led3, timer_3);
}
