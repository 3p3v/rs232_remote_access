set(COMPONENT_REQUIRES sim800l)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src/mqt_pal.c" "src/mqtt.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()