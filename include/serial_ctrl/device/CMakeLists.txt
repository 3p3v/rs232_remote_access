cmake_minimum_required(VERSION 3.0.0)
project(device VERSION 0.1.0 LANGUAGES CXX)
set(DEVICE_TESTS device_tests)

# Main code
add_library(${PROJECT_NAME} INTERFACE)

# Header files
set(DEVICE_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} INTERFACE ${DEVICE_INCLUDE_DIR})

# All extensions
target_link_libraries(${PROJECT_NAME} INTERFACE extensions)

# Extension management
target_link_libraries(${PROJECT_NAME} INTERFACE ext_management)

# Extension management
target_link_libraries(${PROJECT_NAME} INTERFACE sides)

# User control
target_link_libraries(${PROJECT_NAME} INTERFACE user_controller)

# Device record
target_link_libraries(${PROJECT_NAME} INTERFACE dev_record)

# # Tests
# if(${BUILD_TESTING} true)
#     message("EXCHANGER EXTENSION UNIT TESTS FOUND")

#     # Test code
#     add_executable(${DEVICE_TESTS} 
#         "./unit_tests/Exchanger_tests.cpp"
#     )

#     target_include_directories(${DEVICE_TESTS} PUBLIC
#         "./unit_tests/include/"
#     )

#     # target_include_directories(${DEVICE_TESTS} PUBLIC ${TIMERSE_INCLUDE_DIR})
#     # target_link_libraries(${DEVICE_TESTS}  timers) 

#     # extension management
#     target_include_directories(${PROJECT_NAME} PUBLIC ${EXT_MANAGEMENT_INCLUDE_DIR})

#     # Link test library with others
#     target_link_libraries(
#         ${DEVICE_TESTS} 
#         ${PROJECT_NAME}
#         ext_management
#         GTest::gtest_main
#     )

#     set_property(TARGET ${DEVICE_TESTS} PROPERTY CXX_STANDARD 17)

#     # Find tests
#     gtest_discover_tests(${DEVICE_TESTS})
# endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(EXTENSIONS_INCLUDE_DIR 
    ${EXTENSIONS_BASE_INCLUDE_DIR}
    ${EXTENSIONS_COMMONS_INCLUDE_DIR}
    ${LOGIC_EXT_INCLUDE_DIR}
    ${SETTER_EXT_INCLUDE_DIR}
    ${DEVICE_INCLUDE_DIR}
)