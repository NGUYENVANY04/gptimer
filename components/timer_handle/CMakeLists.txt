idf_component_register(SRCS "timer_handle.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver esp_wifi esp_http_server "webserver"  "display_tm1637")
