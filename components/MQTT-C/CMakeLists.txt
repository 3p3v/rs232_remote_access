set(COMPONENT_REQUIRES sim800l mbedtls )#mbedtls_sim800)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src/mqtt_pal.c" "src/mqtt.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()