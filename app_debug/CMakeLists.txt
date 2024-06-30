cmake_minimum_required(VERSION 3.0.0)
project(mqtt_client_debug VERSION 0.1.0 LANGUAGES C CXX)

# Add main
add_executable(${PROJECT_NAME} "./main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE
    "./include/"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    main_modules
    setup_loader
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)