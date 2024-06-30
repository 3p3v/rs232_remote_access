cmake_minimum_required(VERSION 3.0.0)
project(setup_loader VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Setup_defs.cpp
    ./src/Setup_console.cpp
    ./src/setup_impl.cpp
    ./src/setup_loader.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC
    ./include/
)

target_link_libraries(${PROJECT_NAME} PUBLIC defs)

target_link_libraries(${PROJECT_NAME} PUBLIC dev_record)

target_link_libraries(${PROJECT_NAME} PUBLIC extensions)

target_link_libraries(${PROJECT_NAME} PUBLIC main_modules)

target_link_libraries(${PROJECT_NAME} PUBLIC mqtt_port)

# Tests
if(${BUILD_TESTING} true)
    add_subdirectory(./unit_tests/)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(EXTENSIONS_INCLUDE_DIR 
    ${EXTENSIONS_BASE_INCLUDE_DIR}
    ${EXTENSIONS_COMMONS_INCLUDE_DIR}
    ${LOGIC_EXT_INCLUDE_DIR}
    ${SETTER_EXT_INCLUDE_DIR}
    ${DEVICE_INCLUDE_DIR}
)