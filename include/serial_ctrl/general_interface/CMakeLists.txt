cmake_minimum_required(VERSION 3.0.0)
project(general_interface VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Process_manager.cpp"
    "./src/User_controller.cpp"
)

# Header files
set(GENERAL_INTERFACE_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INTERFACE_INCLUDE_DIR})

# Commands interpreter console and jobs
target_include_directories(${PROJECT_NAME} PUBLIC ${CTRL_CONSOLE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ctrl_console)
# Extensions
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  extensions)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)