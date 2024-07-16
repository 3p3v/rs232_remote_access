idf_component_register(
    SRC_DIRS "src/"
    INCLUDE_DIRS "include/"
    REQUIRES 
        error_handler
        mqtt_deamon # FIXME added temporarly
        driver)
