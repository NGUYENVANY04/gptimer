idf_component_register(SRCS "handle_isr.c"
                    INCLUDE_DIRS "include"
                    REQUIRES  driver "savedata" "display_tm1637" "function_keys")
