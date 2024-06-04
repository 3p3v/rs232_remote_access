cmake_minimum_required(VERSION 3.0.0)
project(setter_ext VERSION 0.1.0 LANGUAGES CXX)
set(SETTER_EXT_TESTS setter_ext_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./helpers/src/Remote_record.cpp"
    "./helpers/src/Port_settings_storage.cpp"
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    "./include/"
    "./jobs/"
    "./helpers/include/"
)

# Base extensions
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTENSIONS_BASE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC extensions_base)
# Base extensions
target_include_directories(${PROJECT_NAME} PRIVATE ${EXTENSIONS_COMMONS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC extensions_commons)
# Additional definitions
target_include_directories(${PROJECT_NAME} PRIVATE ${DEFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC defs)

# Tests
if(${BUILD_TESTING} true)
    message("SETTER EXTENSION UNIT TESTS FOUND")

    # Test code
    add_executable(${SETTER_EXT_TESTS} 
        "./unit_tests/Setter_tests.cpp"
    )

    target_include_directories(${SETTER_EXT_TESTS} PRIVATE
        "./unit_tests/include/"
    )

    # extension management
    target_include_directories(${SETTER_EXT_TESTS} PRIVATE ${EXT_MANAGEMENT_INCLUDE_DIR})

    # Link test library with others
    target_link_libraries(
        ${SETTER_EXT_TESTS} 
        ${PROJECT_NAME}
        ext_management
        extension_base_test_hepers
        GTest::gtest_main
    )

    set_property(TARGET ${SETTER_EXT_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${SETTER_EXT_TESTS})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)