idf_component_register(
    SRCS "main.c"
    REQUIRES 
        auth
        error_handler
        mqtt_deamon
        sim_deamon
        uart_deamon)
