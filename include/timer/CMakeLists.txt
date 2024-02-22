cmake_minimum_required(VERSION 3.0.0)
project(timers VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Basic_timer_impl.cpp"
    "./src/Basic_timer.cpp"
    "./src/Timer_cont.cpp"
    "./src/Timer.cpp"
)

# Header files
set(TIMERS_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${TIMERS_INCLUDE_DIR})

# Boost asio
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)