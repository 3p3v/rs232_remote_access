# set(mbedtls_srcs "port/net_sockets_sim800.c")
# # set(mbedtls_include_dirs "mbedtls/include" "mbedtls/library")

# idf_component_register(
#     REQUIRES sim800l
#     SRCS "${mbedtls_srcs}"
#     # INCLUDE_DIRS "${mbedtls_include_dirs}"
#     # PRIV_REQUIRES "${priv_requires}"
#     )

# # Needed to for include_next includes to work from within mbedtls
# # include_directories("${COMPONENT_DIR}/port/include")

# # Import mbedtls library targets
# if (NOT TARGET mbedtls)
# add_subdirectory(mbedtls)
# endif() 

# # Use port specific implementation of net_socket.c instead of one from mbedtls
# get_target_property(src_tls mbedtls SOURCES)
# list(REMOVE_ITEM src_tls net_sockets.c)
# set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})

# # # Change the config file
# # # set(mbedtls_targets mbedtls mbedcrypto mbedx509)
# # # foreach(target ${mbedtls_targets})
# # #     target_compile_definitions(${target} PUBLIC -DMBEDTLS_CONFIG_FILE="mbedtls_config.h")
# # # endforeach()

# # Link libraries together
# target_link_libraries(${COMPONENT_LIB} PUBLIC mbedtls)




set(mbedtls_sim800_srcs "port/net_sockets_sim800.c" "src/mbedtls_sockets.c")

idf_component_register(
    REQUIRES mbedtls sim800l
    SRCS "${mbedtls_sim800_srcs}"
    INCLUDE_DIRS "include"
    )

# Use port specific implementation of net_socket.c instead of one from mbedtls
# get_target_property(src_tls mbedtls SOURCES)
# list(REMOVE_ITEM src_tls net_sockets.c)
# set_property(TARGET mbedtls PROPERTY SOURCES ${src_tls})



