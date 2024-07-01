cmake_minimum_required(VERSION 3.0.0)
project(notyfier VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Notyfication_forwarder.cpp
    ./src/Notyfication_storage.cpp
    ./src/Notyfication_manager.cpp
    ./src/Notyfier.cpp
    ./jobs/src/Debug_job.cpp
    ./jobs/src/Error_job.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC
    ./include/
    ./jobs/include/
)

target_link_libraries(${PROJECT_NAME} PRIVATE ctrl_console)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)