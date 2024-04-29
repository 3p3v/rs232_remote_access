cmake_minimum_required(VERSION 3.0.0)
project(mqtt_client VERSION 0.1.0 LANGUAGES C CXX)
  
# Variables and paths of modules
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_USE_STATIC_LIBS true)
set(PAHO_MQTT_C_LIBRARIES "C:/libs/paho-mqtt-c/win64-1.3.13-ssl/lib/paho-mqtt3as-static.lib")
set(PAHO_MQTT_C_INCLUDE_DIRS "C:/libs/paho-mqtt-c/win64-1.3.13/include")
set(CMAKE_PREFIX_PATH "C:/libs/paho-mqtt-cpp/enhanced/lib/cmake/PahoMqttCpp"
                      "C:/libs/boost/lib/cmake"
                      "c:/libs/gmock/1.14.0-dll/")

# Testing
include(GoogleTest)
enable_testing()
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} "./temp/main.cpp")
# add_executable(${PROJECT_NAME}   #WIN32
#                               # device
#                               src/device/Device.cpp
#                               # serial control objects
#                               src/serial_ctrl/Flow.cpp
#                               src/serial_ctrl/Serial_info.cpp
#                               src/serial_ctrl/Base_serial_ctrl.cpp
#                             #    src/serial_ctrl/Ext_serial_ctrl.cpp
#                               src/serial_ctrl/Serial_ctrl.cpp
#                               src/serial_ctrl/Serial_ctrl_helper.cpp
#                               src/serial_ctrl/Ip_serial_ctrl.cpp
#                               src/serial_ctrl/msgs/Mqtt_msg_cont.cpp
#                               # serial
#                               src/serial/Serial_context.cpp
#                               src/serial/Dyn_serial.cpp
#                               src/serial/Controller.cpp
#                               # ip/mqtt serial
#                               src/ip_serial/Ip_console.cpp
#                               # ip defs
#                               src/ip_serial/definitions/Ip_defs.cpp
#                               src/ip_serial/definitions/Ip_get.cpp
#                               src/ip_serial/definitions/Ip_hi.cpp
#                               src/ip_serial/definitions/Ip_packet.cpp
#                               src/ip_serial/definitions/Ip_packet_flow.cpp
#                               src/ip_serial/definitions/Ip_master.cpp
#                               src/ip_serial/definitions/Ip_slave.cpp
#                               src/ip_serial/definitions/Mqtt_defs.cpp
#                               # main serial
#                               src/main/Controller.cpp
#                               src/main/Dispacher_impl.cpp
#                               src/main/Dispacher.cpp
#                               # setup loader
#                               src/setup_loader/Serial_defs.cpp
#                               src/setup_loader/Setup_loader.cpp
#                               # monitor
#                               src/monitor/Monitor.cpp
#                               # timer
#                               src/timer/Basic_timer.cpp
#                               src/timer/Basic_timer_impl.cpp
#                               src/timer/Timer.cpp
#                               src/timer/Timer_cont.cpp
#                               # exception
#                               src/exception/Exception.cpp
#                               src/exception/setup/Setup_except.cpp
#                               src/exception/mqtt/Mqtt_except.cpp
#                               src/exception/cmds/Cmds_except.cpp
#                               src/exception/cmds/Num_except.cpp
#                               src/exception/serial/Serial_except.cpp
#                               # main
#                               main.cpp)

# target_include_directories(${PROJECT_NAME} PUBLIC "include/serial_ctrl"
#                                                   "include/serial_ctrl/msgs"   
#                                                   "include/serial_ctrl/lr"                                                  
#                                                   "include/device"
#                                                   "include/timer"
#                                                   "include/serial"
#                                                   "include/ip_serial"
#                                                   "include/ip_serial/definitions"
#                                                   "include/exception"
#                                                   "include/exception/setup"
#                                                   "include/exception/mqtt"
#                                                   "include/exception/serial"
#                                                   "include/exception/cmds"
#                                                 #   "include" 
#                                                   "include/main"
#                                                   "include/setup_loader"
#                                                   "include/monitor"
#                                                   "include")

# Find message saving
add_subdirectory("include/serial_ctrl/")
target_include_directories(${PROJECT_NAME} PUBLIC "./temp/")

# # 
# target_include_directories(${PROJECT_NAME} PUBLIC $${PAHO_MQTT_C_INCLUDE_DIRS})

# # find serial port library
set(SERIAL_LIB_NAMES serial_port)
# set(SERIAL_LIB_PATHS "libs/serial_port")
add_subdirectory("libs/serial_port")
# target_include_directories(${PROJECT_NAME} PUBLIC ${serial_port_INCLUDE_DIR})

# # find mqtt_port library
# set(MQTT_PORT_LIB_NAMES mqtt_port)
# set(MQTT_PORT_LIB_PATHS "libs/mqtt_port")
# add_subdirectory("libs/mqtt_port")
# target_include_directories(${PROJECT_NAME} PUBLIC ${mqtt_port_INCLUDE_DIR})

# # find console library
# set(CTRL_CONSOLE_LIB_PATHS "libs/ctrl_console")
add_subdirectory("libs/ctrl_console")
target_include_directories(${PROJECT_NAME} PUBLIC ${CTRL_CONSOLE_INCLUDE_DIR})

# # Link libs
# target_link_libraries(${PROJECT_NAME} PUBLIC serial_port)
# target_link_libraries(${PROJECT_NAME} PUBLIC mqtt_port)
target_link_libraries(${PROJECT_NAME} PUBLIC ctrl_console)
target_link_libraries(${PROJECT_NAME} PUBLIC temp_main)

# # language standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)



# set(wxWidgets_ROOT_DIR "C:/Program Files (x86)/wxWidgets")
# set(wxWidgets_LIB_DIR  "C:/Program Files (x86)/wxWidgets/lib/vc_x64_lib")
#find GUI library
# cmake_policy(SET CMP0054 NEW)
# find_package(wxWidgets REQUIRED)
# include(${wxWidgets_USE_FILE})
# target_include_directories(${PROJECT_NAME} PUBLIC ${wxWidgets_INCLUDE_DIRS})
# message(${wxWidgets_LIBRARIES})
# target_link_libraries(${PROJECT_NAME} PUBLIC ${wxWidgets_LIBRARIES})