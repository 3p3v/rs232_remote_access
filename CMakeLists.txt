cmake_minimum_required(VERSION 3.0.0)
project(mqtt_client VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/Eclipse Paho Cpp/lib/cmake/PahoMqttCpp"
                      "C:/Program Files/wxWidgets-3.2.2.1")

add_executable(${PROJECT_NAME} main.cpp
                               src/Local_serial.cpp
                               src/User_channel_monitor.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "include")

#find GUI library
find_package(wxWidgets REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${wxWidgets_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

# find serial port library
set(SERIAL_LIB_NAMES serial_port)
set(SERIAL_LIB_PATHS "libs/serial_port")
add_subdirectory("libs/serial_port")
target_include_directories(${PROJECT_NAME} PUBLIC ${serial_port_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} serial_port)

# find mqtt_port library
set(MQTT_PORT_LIB_NAMES mqtt_port)
set(MQTT_PORT_LIB_PATHS "libs/mqtt_port")
add_subdirectory("libs/mqtt_port")
target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} mqtt_port)

# find console library
set(CTRL_CONSOLE_LIB_PATHS "libs/ctrl_console")
add_subdirectory("libs/ctrl_console")
target_include_directories(${PROJECT_NAME} PUBLIC ${ctrl_console_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ctrl_console)

# language standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)