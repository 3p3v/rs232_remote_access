set(MAIN_MODULES_TESTS main_modules_tests)

message("MAIN_MODULES UNIT TESTS FOUND")

# Test code
add_executable(${MAIN_MODULES_TESTS} 
    "./General_tests.cpp"
)

target_include_directories(${MAIN_MODULES_TESTS} PRIVATE
    "./include/"
)

# Link test library with others
target_link_libraries(
    ${MAIN_MODULES_TESTS} 
    main_modules
    GTest::gtest_main
)

set_property(TARGET ${MAIN_MODULES_TESTS} PROPERTY CXX_STANDARD 17)

# Find tests
gtest_discover_tests(${MAIN_MODULES_TESTS})