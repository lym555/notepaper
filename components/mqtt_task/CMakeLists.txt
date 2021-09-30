idf_component_register(SRCS "src/mqtt_task.c"
                    INCLUDE_DIRS "include"
                    REQUIRES  nvs_flash 
                    protocol_examples_common
                    mqtt tool
                    )
