idf_component_register(
    SRC_DIRS "src/"
    INCLUDE_DIRS "include/"
    REQUIRES 
        sim800l
        mbedtls_sim800
        mbedtls
        MQTTPacket
        auth
        uart_deamon # FIXME added temporarly
        error_handler
        driver
        esp_event)
