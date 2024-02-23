cmake_minimum_required(VERSION 3.0.0)
project(temp_main VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./extensions/")
add_subdirectory("./timers/")
add_subdirectory("./sides/")
add_subdirectory("./defs/")
add_subdirectory("./general_interface/")
add_subdirectory("./serial/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    extensions
    timers
    sides
    defs
    general_interface
    common_context
)

set(TEMP_MAIN_INCLUDE_DIR 
    ${EXTENSIONS_INCLUDE_DIR}
    ${TIMERS_INCLUDE_DIR}
    ${SIDES_INCLUDE_DIR}
    ${DEFS_INCLUDE_DIR}
    ${GENERAL_INTERFACE_INCLUDE_DIR}
    ${COMMON_CONTEXT_INCLUDE_DIR}
)