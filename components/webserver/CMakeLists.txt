idf_component_register(SRCS "webserver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "savedata" esp_wifi esp_http_server)
