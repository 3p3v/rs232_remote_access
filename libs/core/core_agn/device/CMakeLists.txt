cmake_minimum_required(VERSION 3.0.0)
project(device VERSION 0.1.0 LANGUAGES CXX)
set(DEVICE_TESTS device_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Device_initializer.cpp
    ./src/Device_holder.cpp
)

# Header files
# target_include_directories(${PROJECT_NAME} PRIVATE
#     ./src/
# )
target_include_directories(${PROJECT_NAME} PUBLIC
    ./include/
)

# All extensions
target_link_libraries(${PROJECT_NAME} PUBLIC extensions)

# Main notyfication mechanism
target_link_libraries(${PROJECT_NAME} PUBLIC notyfier)

target_link_libraries(${PROJECT_NAME} PUBLIC extensions_base)

# Extension management
target_link_libraries(${PROJECT_NAME} PUBLIC ext_management)

# Extension management
target_link_libraries(${PROJECT_NAME} PRIVATE sides)

# User control
target_link_libraries(${PROJECT_NAME} PUBLIC user_controller)

# Device record
target_link_libraries(${PROJECT_NAME} PUBLIC dev_record)

# Tests
if(${BUILD_TESTING} true)
    message("DEVICE UNIT TESTS FOUND")

    # Test code
    add_executable(${DEVICE_TESTS} 
        "./unit_tests/Device_tests.cpp"
    )

    target_include_directories(${DEVICE_TESTS} PUBLIC
        "./unit_tests/include/"
    )

    # Link test library with others
    target_link_libraries(
        ${DEVICE_TESTS} 
        ${PROJECT_NAME}
        timers
        sides
        GTest::gtest_main
    )

    set_property(TARGET ${DEVICE_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${DEVICE_TESTS})
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