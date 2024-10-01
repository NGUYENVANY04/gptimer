#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#define SEG_A GPIO_NUM_4
#define SEG_B GPIO_NUM_5
#define SEG_C GPIO_NUM_19
#define SEG_D GPIO_NUM_23
#define SEG_E GPIO_NUM_22
#define SEG_F GPIO_NUM_2
#define SEG_G GPIO_NUM_18
#define SEG_DP GPIO_NUM_21
const uint8_t pin[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
// Mẫu mã cho các chữ số 0-9
const uint8_t digit_patterns[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};
void init_led();
void handle_led();