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

add_subdirectory("./libs/ctrl_console/")
add_subdirectory("./libs/serial_port/")
add_subdirectory("./libs/mqtt_port/")
add_subdirectory("./include/main/")
add_subdirectory("./include/serial_ctrl/")

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