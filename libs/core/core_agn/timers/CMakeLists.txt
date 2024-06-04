cmake_minimum_required(VERSION 3.0.0)
project(timers VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Basic_timer.cpp"
    "./src/Timer_cont.cpp"
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    "./include/"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)