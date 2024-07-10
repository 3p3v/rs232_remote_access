idf_component_register(
    SRC_DIRS "src/"
    INCLUDE_DIRS "include/"
    REQUIRES 
        mqtt_deamon
        nvs_flash
        spiffs)
