cmake_minimum_required(VERSION 3.0.0)
project(main_modules_agn VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Dev_cont.cpp
    ./src/Dev_ext.cpp
    ./src/Dev_getter.cpp
    ./src/Dev_unsafe_getter.cpp
    ./src/Exception_handler.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    ./include/
)

# notyfier
target_link_libraries(${PROJECT_NAME} PUBLIC notyfier)
target_link_libraries(${PROJECT_NAME} PUBLIC device)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)