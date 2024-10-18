#ifndef SAVEDATA_H
#define SAVEDATA_H
#include <stdio.h>
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
extern nvs_handle nvs_handle_timer_1;
extern nvs_handle nvs_handle_timer_2;
extern nvs_handle nvs_handle_timer_3;
extern nvs_handle nvs_handle_timer_4;
extern uint64_t timer_1;
extern uint64_t timer_2;
extern uint64_t timer_3;
extern uint64_t timer_4;

void handle_data();
#endif