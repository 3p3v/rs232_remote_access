cmake_minimum_required(VERSION 3.0.0)
project(user_controller VERSION 0.1.0 LANGUAGES CXX)
set(USER_CONTROLLER_TESTS user_controller_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/User_controller.cpp"
)

# Header files
set(USER_CONTROLLER_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${USER_CONTROLLER_INCLUDE_DIR})

# Commands interpreter console and jobs
target_link_libraries(${PROJECT_NAME} PUBLIC ext_management)
# Extensions
target_link_libraries(${PROJECT_NAME} PRIVATE extensions)

# Tests
if(${BUILD_TESTING} true)
    message("USER CONTROLLER UNIT TESTS FOUND")

    # Test code
    add_executable(${USER_CONTROLLER_TESTS} 
        "./unit_tests/User_controller_tests.cpp"
    )

    target_include_directories(${USER_CONTROLLER_TESTS} PUBLIC
        "./unit_tests/include/"
    )

    # Link test library with others
    target_link_libraries(
        ${USER_CONTROLLER_TESTS} 
        ${PROJECT_NAME}
        ext_management
        extensions
        dev_record
        GTest::gtest_main
    )

    set_property(TARGET ${USER_CONTROLLER_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${USER_CONTROLLER_TESTS})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)