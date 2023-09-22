idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       REQUIRES sim800l
                                mbedtls_sim800
                                mbedtls
                                MQTT-C
                                # mqtt
                                nvs_flash
                                esp_event)
