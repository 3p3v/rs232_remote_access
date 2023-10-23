cmake_minimum_required(VERSION 3.0.0)
project(mqtt_port VERSION 0.1.0 LANGUAGES CXX)

# set policies
cmake_policy(SET CMP0074 NEW)

# set(PAHO_BUILD_STATIC ON)
find_package(PahoMqttCpp REQUIRED)
add_library(${PROJECT_NAME} STATIC
            # src/Receiver.cpp
            src/Sender.cpp
            src/Connector.cpp
            src/Validator.cpp
            src/Basic_controller.cpp
            src/User.cpp
            src/User_opt.cpp
            src/Server.cpp
            src/Callbacks.cpp
            src/impl/User_set_opt.cpp)
            # src/Inter_controller.cpp
            # src/Mqtt_controller.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
# include dirs
set(mqtt_port_INCLUDE_DIR "include" 
                          "include/impl" 
                          "include/fix" 
                          "include/controller"
                          "include/server"
                          "include/user")
# include libs
# target_include_directories(${PROJECT_NAME} PUBLIC ${PahoMqttCpp_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PahoMqttCpp::paho-mqttpp3-static)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)