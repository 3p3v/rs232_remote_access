cmake_minimum_required(VERSION 3.0.0)
project(logic_ext VERSION 0.1.0 LANGUAGES CXX)
set(LOGIC_EXT_TESTS logic_ext_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Logic.cpp"
    "./helpers/src/Remote_status_record.cpp"
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC
    "./include/"
    "./jobs/"
    "./helpers/include/"
)

# Base extensions
target_link_libraries(${PROJECT_NAME} PRIVATE extensions_base)
# 
target_link_libraries(${PROJECT_NAME} PUBLIC extensions_commons)
# Additional definition
target_link_libraries(${PROJECT_NAME} PUBLIC defs)
target_link_libraries(${PROJECT_NAME} PRIVATE ctrl_console)

# Tests
if(${BUILD_TESTING} true)
    message("LOGIC EXTENSION UNIT TESTS FOUND")

    # Test code
    add_executable(${LOGIC_EXT_TESTS} 
        "./unit_tests/logic_tests.cpp"
    )

    target_include_directories(${LOGIC_EXT_TESTS} PRIVATE
        "./unit_tests/include/"
    )

    # extension management
    target_include_directories(${PROJECT_NAME} PRIVATE ${EXT_MANAGEMENT_INCLUDE_DIR})

    # Link test library with others
    target_link_libraries(
        ${LOGIC_EXT_TESTS} 
        ${PROJECT_NAME}
        ext_management
        extension_base_test_hepers
        GTest::gtest_main
    )

    set_property(TARGET ${LOGIC_EXT_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${LOGIC_EXT_TESTS})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)