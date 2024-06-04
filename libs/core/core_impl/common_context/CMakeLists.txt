cmake_minimum_required(VERSION 3.0.0)
project(common_context VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Serial_context.cpp
    ./src/Dyn_serial.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC
    ./include/
)

# Boost asio
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
# Boost serial
target_link_libraries(${PROJECT_NAME} PUBLIC serial_port)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)