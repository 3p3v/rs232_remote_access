cmake_minimum_required(VERSION 3.0.0)
project(extensions_commons VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Remote_conf_port_rec.cpp"
)

# Header files
set(EXTENSIONS_COMMONS_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTENSIONS_COMMONS_INCLUDE_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)