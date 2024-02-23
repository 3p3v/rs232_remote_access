cmake_minimum_required(VERSION 3.0.0)
project(packet_ctrl VERSION 0.1.0 LANGUAGES CXX)
set(PACKET_CTRL_TESTS packet_ctrl_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    # Definitions
    "./logic/definitions/Packet_defs.cpp"
    # Messages store
    "./msgs/packet_container/Mqtt_msg_cont_base.cpp"
    "./msgs/packet_container/Mqtt_msg.cpp"
    "./msgs/packet_container/Mqtt_msg_cont.cpp"
    # Packet counter
    "./msgs/packet_flow/Packet_flow.cpp"
    "./msgs/packet_flow/Packet_master.cpp"
    "./msgs/packet_flow/Packet_slave.cpp"
    # Master packet complete container
    "./msgs/packet_controller/Packet_controller.cpp"
)

# Header files
set(PACKET_CTRL_INCLUDE_DIR 
    # Definitions
    "./logic/definitions/include"
    # Messages store
    "./msgs/packet_container/include/"
    # Packet counter
    "./msgs/packet_flow/include"
    # Master packet complete container
    "./msgs/packet_controller/include"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PACKET_CTRL_INCLUDE_DIR})

if(${BUILD_TESTING} true)
    message("PROTOCOL LOGIC UNIT TESTS FOUND")

    # Test code
    add_executable(${PACKET_CTRL_TESTS} 
        # Messages store
        "./msgs/packet_container/unit_tests/Mqtt_msg_test.cpp"
        # Packet counter
        "./msgs/packet_flow/unit_tests/Packet_flow_test.cpp"
        # Master packet complete container
        "./msgs/packet_controller/unit_tests/Packet_controller_test.cpp")

    # Link test library with others
    target_link_libraries(
        ${PACKET_CTRL_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${PACKET_CTRL_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${PACKET_CTRL_TESTS})
endif()

# Commands interpreter console
target_include_directories(${PROJECT_NAME} PUBLIC ${CTRL_CONSOLE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ctrl_console)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)