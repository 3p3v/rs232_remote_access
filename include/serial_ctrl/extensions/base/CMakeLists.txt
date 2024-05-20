cmake_minimum_required(VERSION 3.0.0)
project(extensions_base VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Remote_ext.cpp"
    "./src/Common_ext.cpp"
    "./src/Authed_ext.cpp"
    "./src/Device_base.cpp"
)

# Header files
set(EXTENSIONS_BASE_INCLUDE_DIR 
    "./include/"
    "./jobs/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_BASE_INCLUDE_DIR})

# notyfier
target_link_libraries(${PROJECT_NAME} PUBLIC notyfier)
# managing workers
target_link_libraries(${PROJECT_NAME} PUBLIC ctrl_console)
# Universal settings and data objects
target_link_libraries(${PROJECT_NAME} INTERFACE sides)
# Timers
target_link_libraries(${PROJECT_NAME} PUBLIC timers)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Unit test helpers
set(EXTENSION_BASE_TEST_HELPERS extension_base_test_hepers)
add_library(${EXTENSION_BASE_TEST_HELPERS} INTERFACE)
target_include_directories(${EXTENSION_BASE_TEST_HELPERS} INTERFACE
    ./unit_tests/include/
)
target_link_libraries(${EXTENSION_BASE_TEST_HELPERS} INTERFACE
    ${PROJECT_NAME}
)