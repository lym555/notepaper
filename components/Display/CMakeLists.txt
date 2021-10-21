idf_component_register(SRCS "src/EPD_2in13_display.c"
                            "src/ImageData.c"
                    INCLUDE_DIRS "include"
                    REQUIRES GUI e-Paper mqtt_task cJSON coap_client
                    
                    )
