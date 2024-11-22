#ifndef FUNCTION_KEYS_H
#define FUNCTION_KEYS_H
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#define DEBOUNCE_DELAY_MS 200
#define LCD_CLK_1 4
#define LCD_DTA_1 5 // TIMER 1

#define LCD_CLK_2 14 // TIMER 2
#define LCD_DTA_2 27

#define LCD_CLK_3 33 // TIMER 3 clock 3
#define LCD_DTA_3 32

#define PIN_RELAY_1 13 // clock 1
#define PIN_RELAY_2 21 // server
#define PIN_RELAY_3 2  // clock 3

#define BTU1 GPIO_NUM_17
#define BTD1 GPIO_NUM_16
#define BTU2 GPIO_NUM_12
#define BTD2 GPIO_NUM_0
extern bool relay_1, relay_3;

extern void congfi_io();
#endif // FUNCTION_KEYS_H
