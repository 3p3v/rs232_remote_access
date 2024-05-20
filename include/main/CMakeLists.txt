cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0 LANGUAGES CXX)
set(MAIN_TESTS main_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Dev_ext.cpp
    ./src/Dev_getter.cpp
    ./src/Dev_adder.cpp
    ./src/Dev_remover.cpp
    ./src/Exception_handler.cpp
    ./src/Debug_monitor.cpp
    ./src/Impl_getter.cpp
    ./src/Impl_adder.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    ./include/
)

# notyfier
target_link_libraries(${PROJECT_NAME} PUBLIC notyfier)
target_link_libraries(${PROJECT_NAME} PRIVATE device_impl)
target_link_libraries(${PROJECT_NAME} PUBLIC device)
target_link_libraries(${PROJECT_NAME} PUBLIC impl)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if(${BUILD_TESTING} true)
    message("MAIN UNIT TESTS FOUND")

    # Test code
    add_executable(${MAIN_TESTS} 
        "./unit_tests/Main_tests.cpp"
    )

    target_include_directories(${MAIN_TESTS} PUBLIC
        "./unit_tests/include/"
    )

    # Link test library with others
    target_link_libraries(
        ${MAIN_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${MAIN_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${MAIN_TESTS})
endif()