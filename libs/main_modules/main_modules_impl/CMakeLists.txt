cmake_minimum_required(VERSION 3.0.0)
project(main_modules_impl VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    ./src/Impl_cont.cpp
    ./src/Impl_ext.cpp
    ./src/Info_getter.cpp
    ./src/Impl_getter.cpp
    ./src/Impl_adder.cpp
    ./src/Impl_adder_ref.cpp
    ./src/Impl_remover.cpp
    ./src/Info_pack_base.cpp
    ./src/Collective_general.cpp
    ./src/Debug_monitor.cpp
    ./src/Impl_starter.cpp
)

# Header files
target_include_directories(${PROJECT_NAME} PUBLIC 
    ./include/
)

target_link_libraries(${PROJECT_NAME} PUBLIC main_modules_agn)
target_link_libraries(${PROJECT_NAME} PUBLIC device_impl)
target_link_libraries(${PROJECT_NAME} PUBLIC sides_impl)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)