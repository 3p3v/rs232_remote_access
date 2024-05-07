cmake_minimum_required(VERSION 3.0.0)
project(dev_record VERSION 0.1.0 LANGUAGES CXX)
set(DEV_RECORD_TESTS dev_record_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Remote_dev.cpp"
    "./src/Remote_port_record.cpp"
    "./src/Serial_port_record.cpp"
)

# Header files
set(DEV_RECORD_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${DEV_RECORD_INCLUDE_DIR})

# Extensions
target_link_libraries(${PROJECT_NAME} extensions)
# Defs
target_link_libraries(${PROJECT_NAME} defs)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)