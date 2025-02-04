cmake_minimum_required(VERSION 3.0.0)
project(proto VERSION 0.1.0 LANGUAGES CXX)
set(PROTO_TESTS proto_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    # "./src/Process_full.cpp"
    # "./src/Ext_id.cpp"
)

# Header files
set(EXT_MANAGEMENT_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXT_MANAGEMENT_INCLUDE_DIR})

# Worker management
target_link_libraries(${PROJECT_NAME} PUBLIC ctrl_console)
target_link_libraries(${PROJECT_NAME} PUBLIC device)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)