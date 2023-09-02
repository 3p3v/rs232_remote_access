# idf_component_register(SRCS "SIM_common_def.c" "LL_SIM.c" "Sim800l.c" "Sim800lBB.c" "Sim800lESP.c"
#                     INCLUDE_DIRS "./include/"
#                     REQUIRES driver)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES driver)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "LL_SIM.c" "SIM_common.c" "SIM_cmd.c" "SIM.c" "SIM_common_def.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()
