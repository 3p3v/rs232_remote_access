cmake_minimum_required(VERSION 3.0.0)
project(defs VERSION 0.1.0 LANGUAGES CXX)

# Main code
add_library(${PROJECT_NAME} STATIC
    "./src/Set_defs.cpp"
)

# Header files
set(DEFS_INCLUDE_DIR 
    "./include/"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${DEFS_INCLUDE_DIR})

# Boost asio
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
# Boost serial
target_include_directories(${PROJECT_NAME} INTERFACE ${serial_port_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} serial_port)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)