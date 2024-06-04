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

# Paths to other CMakeLists
add_subdirectory("./libs/")
add_subdirectory("./app/")
add_subdirectory("./app_debug/")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)