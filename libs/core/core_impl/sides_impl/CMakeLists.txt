cmake_minimum_required(VERSION 3.0.0)
project(sides_impl VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Mqtt_impl.cpp
    ./src/Serial_impl.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    ./include/
)

target_link_libraries(${PROJECT_NAME} PUBLIC dev_record)

# Low-level drivers
target_link_libraries(${PROJECT_NAME} PUBLIC common_context)
target_link_libraries(${PROJECT_NAME} PUBLIC mqtt_port)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)