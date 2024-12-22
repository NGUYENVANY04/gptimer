#ifndef _TIMER_HANDLE_H_
#define _TIMER_HANDLE_H_
#include "driver/gpio.h"
#include <stdbool.h>
#include "display_tm1637.h"
#include "driver/gptimer.h"

extern tm1637_lcd_t *led1;
extern tm1637_lcd_t *led2;
extern tm1637_lcd_t *led3;
extern gptimer_handle_t gptimer_1;
extern gptimer_handle_t gptimer_2;
extern gptimer_handle_t gptimer_3;
extern gptimer_handle_t gptimer_4;

extern bool state_timer_1;
extern bool state_timer_2;
extern bool state_timer_3;
extern bool state_timer_4;

void setup_timer_1(uint64_t timer_1_duration_us);
void setup_timer_2(uint64_t timer_2_duration_us);
void setup_timer_3(uint64_t timer_3_duration_us_on, uint64_t timer_3_duration_us_off, bool free);
void setup_timer_4(uint64_t timer_4_duration_us);
void check_flag_timer(void *par);
void init_data_timer(void);

#endif