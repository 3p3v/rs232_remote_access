cmake_minimum_required(VERSION 3.0.0)
project(ctrl_console VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
            src/Ctrl_console.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
# include dirs
set(ctrl_console_INCLUDE_DIR "include")

target_include_directories(${PROJECT_NAME} PUBLIC ${ctrl_console_INCLUDE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)