idf_component_register(
    SRC_DIRS "src/"
    INCLUDE_DIRS "include/"
    REQUIRES 
        sim800l
        driver
        error_handler
        esp_event)
