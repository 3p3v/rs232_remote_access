cmake_minimum_required(VERSION 3.0.0)
project(serial_port VERSION 0.1.0 LANGUAGES CXX)

# set policies
cmake_policy(SET CMP0074 NEW)

# find boost lib
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} STATIC
            src/Connection.cpp
            src/Serial.cpp)

# include dirs
set(serial_port_INCLUDE_DIR "include")

# include libs
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})   # include boost
target_include_directories(${PROJECT_NAME} PUBLIC ${serial_port_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)