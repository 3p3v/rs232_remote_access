cmake_minimum_required(VERSION 3.0.0)
project(sides VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

set(SIDES_INCLUDE_DIR 
    "./include/"
)

target_include_directories(${PROJECT_NAME} INTERFACE 
    ${SIDES_INCLUDE_DIR}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)