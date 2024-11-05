#include <stdio.h>
#include <driver/gpio.h>
#include "function_keys.h"

void congfi_io(void)
{

    gpio_config_t io_conf_isr = {
        .pin_bit_mask = (1ULL << GPIO_NUM_16 | 1ULL << GPIO_NUM_17 | 1ULL << GPIO_NUM_18 |
                         1ULL << GPIO_NUM_0 | 1ULL << GPIO_NUM_12 | 1ULL << GPIO_NUM_15),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config_t relay = {
        .pin_bit_mask = (1ULL << PIN_RELAY_1 | 1ULL << PIN_RELAY_2 | 1ULL << PIN_RELAY_3), /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_19 |
                         1ULL << GPIO_NUM_23 | 1ULL << GPIO_NUM_34 |
                         1ULL << GPIO_NUM_35),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config_t io_conf_22 = {
        .pin_bit_mask = (1ULL << GPIO_NUM_22),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_reset_pin(GPIO_NUM_14);
    gpio_reset_pin(GPIO_NUM_27);
    gpio_reset_pin(GPIO_NUM_4);
    gpio_reset_pin(GPIO_NUM_5);
    gpio_reset_pin(GPIO_NUM_32);
    gpio_reset_pin(GPIO_NUM_33);
    // gpio_reset_pin(GPIO_NUM_19);
    // gpio_reset_pin(GPIO_NUM_23);
    // gpio_reset_pin(GPIO_NUM_22);
    // gpio_reset_pin(GPIO_NUM_34);
    // gpio_reset_pin(GPIO_NUM_35);

    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_27, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_OUTPUT);

    gpio_config(&io_conf_isr);
    gpio_config(&relay);
    gpio_config(&io_conf);
    gpio_config(&io_conf_22);

    gpio_set_level(PIN_RELAY_1, 0);
    gpio_set_level(PIN_RELAY_2, 0);
    gpio_set_level(PIN_RELAY_3, 0);

    gpio_install_isr_service(0);
}
/*
// Define connections for TM1637 modules
const int CLK1 = 4, DIO1 = 5; LED1
const int CLK2 = 14, DIO2 = 27; LED2
const int CLK4 = 33, DIO4 = 32; LED4



// Button pins for LED1 and LED2
const int BTU1 = 16;  // Increase time for LED1
const int BTD1 = 17;  // Decrease time for LED1
const int BTSET1 = 18; // Set time for LED1 and LED2



// Button pins for LED4
const int BTU4 = 0;  // Increase time for LED4
const int BTD4 = 12;  // Decrease time for LED4
const int BTSET4 = 15; // Set time for LED4

// Relay pins
const int Relay1 = 13;
const int Relay2 = 21;
const int Relay3 = 2;*/