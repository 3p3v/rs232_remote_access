cmake_minimum_required(VERSION 3.0.0)
project(ctrl_console VERSION 0.1.0 LANGUAGES CXX)
set(CTRL_CONSOLE_TESTS ctrl_console_tests)

add_library(${PROJECT_NAME} STATIC
            src/Ctrl_console.cpp
            src/Common_defs.cpp
            src/Ctrl_con_defs.cpp
            # Policies
            src/Numbers_only.cpp
            src/Alpha_only.cpp
            src/Upper_only.cpp
            src/Floor_only.cpp
            src/Is_cmd.cpp)


if(${BUILD_TESTING} true)
    message("CTRL CONSOLE UNIT TESTS FOUND")

    # Test code
    add_executable(${CTRL_CONSOLE_TESTS} 
        # base ctrl console
        "./unit_tests/Base_ctrl_console_test.cpp"
        # policies
        "./unit_tests/policies/Numbers_only_test.cpp"
        "./unit_tests/policies/Alpha_only_test.cpp"
        "./unit_tests/policies/Floor_only_test.cpp"
        "./unit_tests/policies/Equal_to_test.cpp"
        "./unit_tests/policies/Lower_than_test.cpp"
        "./unit_tests/policies/Bigger_than_test.cpp"
        "./unit_tests/policies/Or_test.cpp"
        "./unit_tests/policies/Bigger_or_equal_test.cpp"
        "./unit_tests/policies/Lower_or_equal_test.cpp"
        "./unit_tests/policies/Is_cmd_test.cpp"
    )

    # Link test library with others
    target_link_libraries(
        ${CTRL_CONSOLE_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${CTRL_CONSOLE_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${CTRL_CONSOLE_TESTS})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
# include dirs
set(CTRL_CONSOLE_INCLUDE_DIR "include"
                             "include/policies")

target_include_directories(${PROJECT_NAME} PUBLIC ${CTRL_CONSOLE_INCLUDE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)