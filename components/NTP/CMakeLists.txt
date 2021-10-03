idf_component_register(SRCS "ntp.c"
                    "timestamp_compare.c"
                    "timestamp_format.c"
                    "timestamp_parse.c"
                    "timestamp_tm.c"
                    "timestamp_valid.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash 
                    protocol_examples_common
                    Display
                    cJSON
                    )
