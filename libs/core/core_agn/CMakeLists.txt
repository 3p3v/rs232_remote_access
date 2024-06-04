cmake_minimum_required(VERSION 3.0.0)
project(core_agn VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./ctrl_console/")
add_subdirectory("./extensions/")
add_subdirectory("./timers/")
add_subdirectory("./sides/")
add_subdirectory("./defs/")
add_subdirectory("./ext_management/")
add_subdirectory("./user_controller/")
add_subdirectory("./device/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    ctrl_console
    extensions
    timers
    sides
    defs
    ext_management
    device
)