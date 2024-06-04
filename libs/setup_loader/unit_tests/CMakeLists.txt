set(SETUP_LOADER_TESTS setup_loader_tests)

message("SETUP LOADER UNIT TESTS FOUND")

# Test code
add_executable(${SETUP_LOADER_TESTS} 
    ./src/Setup_loader_tests.cpp
)

# Link test library with others
target_link_libraries(
    ${SETUP_LOADER_TESTS} 
    setup_loader
    GTest::gtest_main
)

set_property(TARGET ${SETUP_LOADER_TESTS} PROPERTY CXX_STANDARD 17)

# Copy conf file for debugging
file(COPY ./resources/test_setup.conf DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/")

# Find tests
gtest_discover_tests(
    ${SETUP_LOADER_TESTS}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/"
)