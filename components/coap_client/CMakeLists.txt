idf_component_register(SRCS "weather_now.c" "weather_3d.c" "ntp.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash 
                    protocol_examples_common
                    Display
                    cJSON
                    coap
                    EMBED_TXTFILES certs/coap_ca.pem certs/coap_client.crt certs/coap_client.key
                    )
