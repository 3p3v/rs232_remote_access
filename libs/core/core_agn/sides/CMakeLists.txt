cmake_minimum_required(VERSION 3.0.0)
project(sides VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    ./src/Intf_except.cpp
    ./src/Mqtt_except.cpp
    ./src/Serial_except.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ./include/
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)