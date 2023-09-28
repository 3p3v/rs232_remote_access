idf_component_register(#SRCS "main.c"
                       SRC_DIRS "src"
                       INCLUDE_DIRS "include/"
                       REQUIRES sim800l
                                mbedtls_sim800
                                mbedtls
                                MQTT-C
                                # mqtt
                                nvs_flash
                                esp_event)
