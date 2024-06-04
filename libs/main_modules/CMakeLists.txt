cmake_minimum_required(VERSION 3.0.0)
project(main_modules VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory("./main_modules_agn/")
add_subdirectory("./main_modules_impl/")

add_library(${PROJECT_NAME} INTERFACE)

# notyfier
target_link_libraries(${PROJECT_NAME} INTERFACE main_modules_agn)
target_link_libraries(${PROJECT_NAME} INTERFACE main_modules_impl)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if(${BUILD_TESTING} true)
    add_subdirectory("./unit_tests/")
endif()