cmake_minimum_required(VERSION 3.0.0)
project(mqtt_port VERSION 0.1.0 LANGUAGES CXX)

# set policies
cmake_policy(SET CMP0074 NEW)

# set(PAHO_BUILD_STATIC ON)
find_package(PahoMqttCpp REQUIRED)
find_package(OpenSSL REQUIRED)
add_library(${PROJECT_NAME} STATIC
            # src/Receiver.cpp
            # src/Basic_controller.cpp
            src/Callb.cpp
            # src/Channel_conn_callb_impl.cpp
            # src/Channel_conn_callb.cpp
            # src/Conn_callb_impl.cpp
            # src/Conn_callb.cpp
            # src/Connector.cpp
            # src/Controller.cpp
            src/C_callb.cpp
            src/O_callb.cpp
            # src/Device.cpp
            src/Send_callb_impl.cpp
            # src/Validator.cpp
            src/impl/Controller.cpp
            src/impl/Sub.cpp
            src/impl/Pub.cpp
            src/impl/Impl_server.cpp
            src/impl/Impl_user.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
# include dirs
set(mqtt_port_INCLUDE_DIR "include" 
                          "include/basic_callbacks" 
                          "include/callbacks" 
                        #   "include/controller"
                          "include/impl"
                          "include/impl/basic_callbacks"
                          "include/impl/delay"
                          "include/impl/callbacks"
                          "include/impl/emulated_callbacks"
                          "include/server"
                          "include/user"
                          "include/device")
# include libs
# target_include_directories(${PROJECT_NAME} PUBLIC ${PahoMqttCpp_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC PahoMqttCpp::paho-mqttpp3-static)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)