# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(COMPONENTS esp32 esptool_py bootloader main spi_flash DRIVER mbedtls mqtt) # lwip)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(praca_inzynierska_device)
