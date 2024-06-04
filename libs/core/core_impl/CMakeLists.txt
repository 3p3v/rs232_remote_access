cmake_minimum_required(VERSION 3.0.0)
project(core_impl VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./mqtt_port/")
add_subdirectory("./serial_port/")
add_subdirectory("./timers_impl/")
add_subdirectory("./dev_record/")
add_subdirectory("./common_context/")
add_subdirectory("./sides_impl/")
add_subdirectory("./device_impl/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    mqtt_port
    serial_port
    timers_impl
    dev_record
    common_context
    device_impl
    sides_impl
)