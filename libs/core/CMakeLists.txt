cmake_minimum_required(VERSION 3.0.0)
project(core VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./core_agn/")
add_subdirectory("./core_impl/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    core_agn
    core_impl
)