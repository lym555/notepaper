idf_component_register(SRCS "coap_weather.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash 
                    protocol_examples_common
                    Display
                    cJSON
                    coap
                    EMBED_TXTFILES certs/coap_ca.pem certs/coap_client.crt certs/coap_client.key
                    )
