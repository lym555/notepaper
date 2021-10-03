idf_component_register(SRCS "weather.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash 
                    protocol_examples_common
                    Display
                    cJSON
                    )
