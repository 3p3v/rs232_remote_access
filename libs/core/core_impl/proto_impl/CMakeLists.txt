cmake_minimum_required(VERSION 3.0.0)
project(device_impl VERSION 0.1.0 LANGUAGES CXX)
set(DEVICE_IMPL_TESTS device_impl_tests)

# Main code
add_library(${PROJECT_NAME} INTERFACE)

# Header files
set(DEVICE_IMPL_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${DEVICE_IMPL_INCLUDE_DIR})

# Implementation-agostic device
target_link_libraries(${PROJECT_NAME} INTERFACE device)

# Low-level drivers
target_link_libraries(${PROJECT_NAME} INTERFACE sides_impl)
target_link_libraries(${PROJECT_NAME} INTERFACE timers_impl)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
