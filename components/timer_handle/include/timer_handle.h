#ifndef _TIMER_HANDLE_H_
#define _TIMER_HANDLE_H_

void setup_timer_1(uint64_t timer_1_duration_us);
void setup_timer_2(uint64_t timer_2_duration_us);
void setup_timer_3(uint64_t timer_3_duration_us);
void setup_timer_4(uint64_t timer_3_duration_us);
void check_flag_timer(void *par);
void init_data_timer(void);

#endif