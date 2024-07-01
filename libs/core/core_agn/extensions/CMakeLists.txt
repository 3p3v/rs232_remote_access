cmake_minimum_required(VERSION 3.0.0)
project(extensions VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./logic/")
add_subdirectory("./base/")
add_subdirectory("./commons/")
add_subdirectory("./setter/")
add_subdirectory("./exchanger/")
add_subdirectory("./ext_except/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    logic_ext
    setter_ext
    exchanger_ext
)