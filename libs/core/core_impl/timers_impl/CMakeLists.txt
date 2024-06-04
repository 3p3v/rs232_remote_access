cmake_minimum_required(VERSION 3.0.0)
project(timers_impl VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Basic_timer_impl.cpp"
    # "./src/Timer.cpp"
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    "./include/"
)

# Boost asio
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

# Implementation-agnostic timer
target_link_libraries(${PROJECT_NAME} PUBLIC timers)

# Context
target_link_libraries(${PROJECT_NAME} PRIVATE common_context)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)