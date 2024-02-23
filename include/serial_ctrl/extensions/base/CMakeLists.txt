cmake_minimum_required(VERSION 3.0.0)
project(extensions_base VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Remote_ext.cpp"
    "./src/Unauthed_ext.cpp"
    "./src/Common_ext.cpp"
    "./src/Authed_ext.cpp"
)

# Header files
set(EXTENSIONS_BASE_INCLUDE_DIR 
    "./include/"
    "./jobs/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_BASE_INCLUDE_DIR})

# 
target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INTERFACE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  general_interface)
# Universal settings and data objects
target_include_directories(${PROJECT_NAME} PUBLIC ${SIDES_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  sides)
# Timers
target_include_directories(${PROJECT_NAME} PUBLIC ${TIMERS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  timers)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)