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

void handle_data();
#endif