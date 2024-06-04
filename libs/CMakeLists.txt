cmake_minimum_required(VERSION 3.0.0)
project(libs VERSION 0.1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory("./core/")
add_subdirectory("./main_modules/")
add_subdirectory("./setup_loader/")

# Link all modules
target_link_libraries(${PROJECT_NAME} INTERFACE
    core
    main_modules
    setup_loader
)