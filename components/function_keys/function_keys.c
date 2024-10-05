#include <stdio.h>
#include <driver/gpio.h>
#include "function_keys.h"

void congfi_io(void)
{

    gpio_config_t io_conf_isr = {
        .pin_bit_mask = (1ULL << GPIO_NUM_16 | 1ULL << GPIO_NUM_17 | 1ULL << GPIO_NUM_18), .mode = GPIO_MODE_INPUT, .pull_up_en = GPIO_PULLUP_ENABLE, .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE, // Interrupt on falling edge
    };

    // GPIO configuration for LED (GPIO 2)
    gpio_config_t io_conf_led = {
        .pin_bit_mask = (1ULL << GPIO_NUM_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&io_conf_isr); // Configure button GPIO
    gpio_config(&io_conf_led); // Configure LED GPIO
}
