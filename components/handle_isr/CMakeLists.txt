idf_component_register(SRCS "handle_isr.c"
                    INCLUDE_DIRS "include"
                    REQUIRES  driver "display_tm1637" "handle_isr" "function_keys")
