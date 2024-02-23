cmake_minimum_required(VERSION 3.0.0)
project(logic_ext VERSION 0.1.0 LANGUAGES CXX)
set(LOGIC_EXT_TESTS extensions_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Logic.cpp"
)

# Header files
set(LOGIC_EXT_INCLUDE_DIR 
    "./include/"
    "./helpers/"
    "./jobs/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${LOGIC_EXT_INCLUDE_DIR})

if(${BUILD_TESTING} true)
    message("LOGIC EXTENSION UNIT TESTS FOUND")

    # Test code
    add_executable(${LOGIC_EXT_TESTS} 
        "./unit_tests/logic_tests.cpp"
    )

    target_include_directories(${LOGIC_EXT_TESTS}
        "./unit_tests/include/"
    )

    # Link test library with others
    target_link_libraries(
        ${LOGIC_EXT_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${LOGIC_EXT_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${LOGIC_EXT_TESTS})
endif()

# Base extensions
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_BASE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  extensions_base)
# Base extensions
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_COMMONS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  extensions_commons)
# Additional definitions
target_include_directories(${PROJECT_NAME} PUBLIC ${DEFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  defs)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)