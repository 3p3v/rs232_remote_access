cmake_minimum_required(VERSION 3.0.0)
project(setter_ext VERSION 0.1.0 LANGUAGES CXX)
set(SETTER_EXT_TESTS extensions_tests)

# Main code
add_library(${PROJECT_NAME} INTERFACE)

# Header files
set(SETTER_EXT_INCLUDE_DIR 
    "./include/"
    "./helpers/"
    "./jobs/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${SETTER_EXT_INCLUDE_DIR})

# if(${BUILD_TESTING} true)
#     message("SETTER EXTENSION UNIT TESTS FOUND")

#     # Test code
#     add_executable(${SETTER_EXT_TESTS} 
#         "./unit_tests/setter_tests.cpp"
#     )

#     # Link test library with others
#     target_link_libraries(
#         ${SETTER_EXT_TESTS} 
#         ${PROJECT_NAME}
#         GTest::gtest_main
#     )

#     set_property(TARGET ${SETTER_EXT_TESTS} PROPERTY CXX_STANDARD 17)

#     # Find tests
#     gtest_discover_tests(${SETTER_EXT_TESTS})
# endif()

# Base extensions
target_include_directories(${PROJECT_NAME} INTERFACE ${EXTENSIONS_BASE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE extensions_base)
# Base extensions
target_include_directories(${PROJECT_NAME} INTERFACE ${EXTENSIONS_COMMONS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE extensions_commons)
# Additional definitions
target_include_directories(${PROJECT_NAME} INTERFACE ${DEFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} INTERFACE defs)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)