# idf_component_register(SRCS "SIM_common_def.c" "LL_SIM.c" "Sim800l.c" "Sim800lBB.c" "Sim800lESP.c"
#                     INCLUDE_DIRS "./include/"
#                     REQUIRES driver)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES driver freertos esp_hw_support heap log soc hal esp_rom esp_common esp_system xtensa)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src/LL_SIM.c" "src/SIM_common.c" "src/SIM.c" "src/SIM_common_def.c" 
                   "SIM/TCPIP/SIM_TCPIP.c" "lwip_sim800/src/inet_sim800.c" )
set(COMPONENT_ADD_INCLUDEDIRS "include" "SIM/TCPIP/include" "lwip_sim800/include" )

register_component()

# add_definitions(-DWOLFSSL_USER_SETTINGS -DWOLFSSL_ESPWROOM32SE)
# Import mbedtls library targets
# add_subdirectory(encryption/wolfssl)

# Use port specific implementation of net_socket.c instead of one from mbedtls
# get_target_property(src_tls mbedtls SOURCES)
# list(REMOVE_ITEM src_tls net_sockets.c)
# set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

# # Change the config file
# # set(mbedtls_targets mbedtls mbedcrypto mbedx509)
# # foreach(target ${mbedtls_targets})
# #     target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls_config.h")
# # endforeach()

# Link libraries together
# target_link_libraries(${COMPONENT_LIB} PUBLIC wolfssl)