#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include <sys/time.h>
#include <esp_system.h>
#include "handle_isr.h"
#include "display_tm1637.h"
#include "function_keys.h"
#include "savedata.h"
#include "webserver.h"
#include "handle_setting_timer_3.h"
#include "timer_handle.h"
#define TAG "app"

// Callback function to handle timer alarm event
bool state_setting = false;
void stop_services_callback(TimerHandle_t xTimer)
{
    ESP_LOGI("TIMER", "Stopping Wi-Fi and HTTP server...");

    esp_wifi_stop();
    if (server != NULL)
    {
        httpd_stop(server);
    }
    ESP_LOGI("TIMER", "Services stopped.");
}
void app_main(void)
{
    gpio_reset_pin(GPIO_NUM_14);
    gpio_reset_pin(GPIO_NUM_27);

    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
    static TimerHandle_t stop_services_timer;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    congfi_io();
    handle_data();

    init_setting_timer_1_2();
    init_setting_timer_3();
    congfi_timer_4();
    stop_services_timer = xTimerCreate("StopServicesTimer", pdMS_TO_TICKS(60000), pdFALSE, NULL, stop_services_callback);

    if (stop_services_timer != NULL)
    {

        xTimerStart(stop_services_timer, 0);
        ESP_LOGI("APP", "Timer started to stop services after 2 minutes.");
    }
    else
    {
        ESP_LOGE("APP", "Failed to create timer.");
    }
    xTaskCreate(check_flag_timer, "check timer flag", 4096, NULL, 10, NULL);
}
