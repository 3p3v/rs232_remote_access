cmake_minimum_required(VERSION 3.0.0)
project(ext_management VERSION 0.1.0 LANGUAGES CXX)
set(EXT_MANAGEMENT_TESTS ext_management_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Process_full.cpp"
    # "./src/Shared_ext.cpp"
    "./src/Ext_id.cpp"
    "./src/Ws_holder.cpp"
)

# Header files
set(EXT_MANAGEMENT_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXT_MANAGEMENT_INCLUDE_DIR})

# Worker management
target_link_libraries(${PROJECT_NAME} PRIVATE ctrl_console)
# Extensions
target_link_libraries(${PROJECT_NAME} PRIVATE extensions_base)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)