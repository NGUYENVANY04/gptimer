#include "savedata.h"
#include "timer_handle.h"
#include "esp_log.h"
#include "display_tm1637.h"
#include "function_keys.h"
uint64_t timer_1 = 0, timer_2 = 0, timer_3_on = 0, timer_3_off = 0, timer_4 = 0;
nvs_handle nvs_handle_timer_1;
nvs_handle nvs_handle_timer_2;
nvs_handle nvs_handle_timer_3;
nvs_handle nvs_handle_timer_4;

void handle_data()
{

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
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", timer_3_on));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
        nvs_close(nvs_handle_timer_3);
        ESP_ERROR_CHECK(nvs_open("timer_3", NVS_READWRITE, &nvs_handle_timer_3));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", timer_3_off));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_3));
        nvs_close(nvs_handle_timer_3);

        ESP_ERROR_CHECK(nvs_open("timer_4", NVS_READWRITE, &nvs_handle_timer_4));
        ESP_ERROR_CHECK(nvs_set_u64(nvs_handle_timer_4, "TIMER_4_KEY", timer_4));
        ESP_ERROR_CHECK(nvs_commit(nvs_handle_timer_4));
        nvs_close(nvs_handle_timer_4);

        // Re-open the namespace in read-only mode
    }
    esp_err_t err_off = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_OFF", &timer_3_off);
    esp_err_t err_on = nvs_get_u64(nvs_handle_timer_3, "TIMER_3_KEY_ON", &timer_3_on);
    nvs_close(nvs_handle_timer_1);
    nvs_close(nvs_handle_timer_2);
    nvs_close(nvs_handle_timer_3);
    nvs_close(nvs_handle_timer_4);

    // Print the timers

    // Check the validity of the timers and set up if needed
    // if (err_timer_1 == ESP_OK && err_timer_2 == ESP_OK &&
    //     timer_1 > 0 && timer_2 > 0 && timer_3 > 0 && timer_4 > 0)
    // {
    //     setup_timer_1(timer_1 * 60000000);
    //     setup_timer_2(timer_2 * 60000000);
    //     setup_timer_3(timer_3 * 60000000);
    //     setup_timer_4(timer_4 * 1000000);
    // }
    // else
    // {
    //     printf("No valid timers found.\n");
    // }
}
