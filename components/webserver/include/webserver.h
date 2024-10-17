#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
extern httpd_config_t config;
extern httpd_handle_t server;
void wifi_init_softap();
void start_webserver();
void congfi_timer_4();
#endif