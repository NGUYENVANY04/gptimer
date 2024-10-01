#ifndef FUNCTION_KEYS_H
#define FUNCTION_KEYS_H
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "driver/gpio.h"
#include "handle_isr.h"
#define RESET_BUTTON_PIN GPIO_NUM_39 // GPIO for the reset button
#define STATE_WIFI GPIO_NUM_12
#define ADC_INPUT GPIO_NUM_5
#define ADC_WIDTH ADC_WIDTH_BIT_12 // Use 12-bit resolution
#define ADC_ATTEN ADC_ATTEN_DB_0   // No attenuation

#define DEBOUNCE_DELAY_MS 500

// Khai báo các hàm
// void init_button_reset(void);
// void init_led_state_wifi(void);
// void init_button_control(void *arg);
// // void init_button_control_connect_server(void *client);
// void init_state_output(void);
// // void led_task(void *arg);
// // void led_isr_handler(void *arg);
// // void button_isr_handler(void *arg);
// void init_led_state_mqtt();
void init_state_output();
#endif // FUNCTION_KEYS_H
