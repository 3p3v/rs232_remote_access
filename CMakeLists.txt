cmake_minimum_required(VERSION 3.0.0)
project(mqtt_client VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/Eclipse Paho Cpp/lib/cmake/PahoMqttCpp")

add_executable(${PROJECT_NAME} main.cpp
                               src/Local_serial.cpp
                               src/User_channel_monitor.cpp)

# find serial port library
set(SERIAL_LIB_NAMES serial_port)
set(SERIAL_LIB_PATHS "libs/serial_port")
add_subdirectory("libs/serial_port")
# find_library(SERIAL_LIB
#              NAMES ${SERIAL_LIB_NAMES}
#              PATHS ${SERIAL_LIB_PATHS} 
#              NO_CACHE
#              REQUIRED)           

# if(SERIAL_LIB STREQUAL SERIAL_LIB-NOTFOUND)
#     message(SEND_ERROR "Serial port library not found")
# else()
#     message(STATUS "Serial port library found")
# endif()

set(MQTT_PORT_LIB_NAMES mqtt_port)
set(MQTT_PORT_LIB_PATHS "libs/mqtt_port")
add_subdirectory("libs/mqtt_port")





# include libs
target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_include_directories(${PROJECT_NAME} PUBLIC ${serial_port_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} serial_port)
target_link_libraries(${PROJECT_NAME} mqtt_port)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)