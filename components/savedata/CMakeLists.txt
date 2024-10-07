idf_component_register(SRCS "savedata.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "function_keys"  "display_tm1637" "timer_handle" nvs_flash driver)
