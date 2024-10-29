#ifndef _TIMER_HANDLE_H_
#define _TIMER_HANDLE_H_
#include "driver/gpio.h"
#include <stdbool.h>
void setup_timer_1(uint64_t timer_1_duration_us);
void setup_timer_2(uint64_t timer_2_duration_us);
void setup_timer_3(uint64_t timer_3_duration_us_on, uint64_t timer_3_duration_us_off, bool free);
void setup_timer_delay(uint64_t delay_1, uint64_t delay_2, gpio_num_t pin);
void check_flag_timer(void *par);
void init_data_timer(void);

#endif