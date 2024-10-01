#include <stdio.h>
#include "display_led.h"

void handle_led(int j)
{

    for (int i = 0; i < 7; i++)
    {
        gpio_set_level(pin[i], (digit_patterns[j]) & (1 << (i)));
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}