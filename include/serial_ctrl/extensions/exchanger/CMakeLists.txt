cmake_minimum_required(VERSION 3.0.0)
project(exchanger_ext VERSION 0.1.0 LANGUAGES CXX)
set(EXCHANGER_EXT_TESTS exchanger_ext_tests)

# Main code
add_library(${PROJECT_NAME} INTERFACE)

# Header files
set(EXCHANGER_EXT_INCLUDE_DIR 
    "./include/"
    "./helpers/"
    "./jobs/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${EXCHANGER_EXT_INCLUDE_DIR})

# Base extensions
target_include_directories(${PROJECT_NAME} INTERFACE ${EXTENSIONS_BASE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE extensions_base)
# Base extensions
target_include_directories(${PROJECT_NAME} INTERFACE ${EXTENSIONS_COMMONS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE extensions_commons)
# Additional definitions
target_include_directories(${PROJECT_NAME} INTERFACE ${DEFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE defs)

# Add message control
add_subdirectory("./msgs/")
target_link_libraries(${PROJECT_NAME} INTERFACE
    packet_ctrl
)
target_include_directories(${PROJECT_NAME} INTERFACE ${PACKET_CTRL_INCLUDE_DIR})

# Tests
if(${BUILD_TESTING} true)
    message("EXCHANGER EXTENSION UNIT TESTS FOUND")

    # Test code
    add_executable(${EXCHANGER_EXT_TESTS} 
        "./unit_tests/Exchanger_tests.cpp"
    )

    target_include_directories(${EXCHANGER_EXT_TESTS} PUBLIC
        "./unit_tests/include/"
    )

    # target_include_directories(${EXCHANGER_EXT_TESTS} PUBLIC ${TIMERSE_INCLUDE_DIR})
    # target_link_libraries(${EXCHANGER_EXT_TESTS}  timers) 

    # Link test library with others
    target_link_libraries(
        ${EXCHANGER_EXT_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${EXCHANGER_EXT_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${EXCHANGER_EXT_TESTS})
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
    ${EXCHANGER_EXT_INCLUDE_DIR}
)