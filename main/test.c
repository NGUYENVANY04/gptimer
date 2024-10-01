
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include <inttypes.h>

#define LED 2
#define Push_Button 4

bool gpio_value = false;

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    /*
        GOOD to DO - Implement the debouncing algorithm
        to eliminate the Spurious interrupt trigger. (already done)
    */

    /* Disable the Interrupt */
    gpio_intr_disable(Push_Button);       // Disable the Interupt on the LED Pin
    gpio_isr_handler_remove(Push_Button); // Remove the interupt service routine on the Push button

    /* Button is pressed. Toggle the LED */
    gpio_set_level(LED, gpio_value); // Set the level of the LED pin
    gpio_value = !gpio_value;        // Reset the level of the LED Pin

    /* Re-Enable the Interrupt */
    gpio_isr_handler_add(Push_Button, gpio_isr_handler, NULL); // Enable the interupt routine on the push button
    gpio_intr_enable(Push_Button);                             // Enable the interupt on the Push button
}

void app_main(void)
{
    /* Reset the pin */
    gpio_reset_pin(LED);         // Reset the LED Pin
    gpio_reset_pin(Push_Button); // Reset the PushButton Pin

    /* Set the GPIOs to Output mode */
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);        // Set the GPIO MODE of the LED as ouyput
    gpio_set_direction(Push_Button, GPIO_MODE_INPUT); // Set the GPIO Mode of the Push button as input

    /* Enable Pullup for Input Pin */
    gpio_pullup_en(Push_Button); // Enable PullUp

    /* Disable pulldown for Input Pin */
    gpio_pulldown_dis(Push_Button);

    /* Configure Raising Edge detection Interrupt for Input Pin */
    gpio_set_intr_type(Push_Button, GPIO_INTR_POSEDGE); // Set the interupt on the GPIO as positive edge or rising edge

    /* install gpio isr service to default values */
    gpio_install_isr_service(0);

    /* Attach the ISR to the GPIO interrupt */
    gpio_isr_handler_add(Push_Button, gpio_isr_handler, NULL);

    /* Enable the Interrupt */
    gpio_intr_enable(Push_Button);

    while (1)
    {
        if (gpio_get_level(LED))
        {
            printf("ON \n");
        }
        else
        {
            printf("OFF \n");
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}