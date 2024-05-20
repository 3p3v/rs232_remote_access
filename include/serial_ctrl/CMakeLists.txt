cmake_minimum_required(VERSION 3.0.0)
project(temp_main VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./extensions/")
add_subdirectory("./timers/")
add_subdirectory("./sides/")
add_subdirectory("./defs/")
add_subdirectory("./ext_management/")
add_subdirectory("./dev_record/")
add_subdirectory("./user_controller/")
add_subdirectory("./serial/")
add_subdirectory("./impl/")
add_subdirectory("./device/")
add_subdirectory("./device_impl/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    extensions
    timers
    sides
    defs
    ext_management
    dev_record
    common_context
    device
    device_impl
    impl
)

set(TEMP_MAIN_INCLUDE_DIR 
    ${EXTENSIONS_INCLUDE_DIR}
    ${TIMERS_INCLUDE_DIR}
    ${SIDES_INCLUDE_DIR}
    ${DEFS_INCLUDE_DIR}
    ${EXT_MANAGEMENT_INCLUDE_DIR}
    ${COMMON_CONTEXT_INCLUDE_DIR}
    ${DEVICE_INCLUDE_DIR}
    ${DEVICE_IMPL_INCLUDE_DIR}
    ${IMPL_INCLUDE_DIR}
)