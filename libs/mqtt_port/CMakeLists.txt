cmake_minimum_required(VERSION 3.0.0)
project(mqtt_port VERSION 0.1.0 LANGUAGES CXX)

# set policies
cmake_policy(SET CMP0074 NEW)

# find boost lib
# set(PAHO_BUILD_STATIC ON)
find_package(PahoMqttCpp REQUIRED)

add_library(${PROJECT_NAME} STATIC
            src/Traffic_receiver.cpp
            src/Traffic_controller.cpp)

# include dirs
set(mqtt_port_INCLUDE_DIR "include")

# include libs
target_include_directories(${PROJECT_NAME} PUBLIC ${PahoMqttCpp_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${PahoMqttCpp_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)