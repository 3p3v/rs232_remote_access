cmake_minimum_required(VERSION 3.0.0)
project(common_context VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} INTERFACE)

# Header files
set(COMMON_CONTEXT_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${COMMON_CONTEXT_INCLUDE_DIR})

# Boost asio
target_include_directories(${PROJECT_NAME} INTERFACE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE ${Boost_LIBRARIES})
# Boost serial
target_include_directories(${PROJECT_NAME} INTERFACE ${serial_port_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE serial_port)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)