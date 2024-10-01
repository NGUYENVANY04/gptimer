#ifndef HANDLE_ISR_H
#define HANDLE_ISR_H
#include "function_keys.h"
TaskHandle_t task_setting = NULL;
TaskHandle_t toggle_option = NULL;
void option(void *arg);
void setting(void *arg);
void option_isr_handler(void *arg);
void setting_isr_handler(void *arg);
#endif