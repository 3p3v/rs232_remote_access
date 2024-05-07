cmake_minimum_required(VERSION 3.0.0)
project(worker_management VERSION 0.1.0 LANGUAGES CXX)
set(WORKER_MANAGEMENT_TESTS worker_management_tests)

# Main code
add_library(${PROJECT_NAME} INTERFACE
)

# Header files
set(WORKER_MANAGEMENT_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${WORKER_MANAGEMENT_INCLUDE_DIR})

# Commands interpreter console and jobs
target_include_directories(${PROJECT_NAME} INTERFACE ${CTRL_CONSOLE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE ctrl_console)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)