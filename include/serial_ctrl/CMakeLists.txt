cmake_minimum_required(VERSION 3.0.0)
project(protocol_logic VERSION 0.1.0 LANGUAGES CXX)
set(PROTOCOL_LOGIC_TESTS protocol_logic_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    # Definitions
    "./logic/definitions/Packet_defs.cpp"
    # Messages store
    "./extensions/helpers/msgs/packet_container/Mqtt_msg_cont_base.cpp"
    "./extensions/helpers/msgs/packet_container/Mqtt_msg.cpp"
    "./extensions/helpers/msgs/packet_container/Mqtt_msg_cont.cpp"
    # Packet counter
    "./extensions/helpers/msgs/packet_flow/Packet_flow.cpp"
    "./extensions/helpers/msgs/packet_flow/Packet_master.cpp"
    "./extensions/helpers/msgs/packet_flow/Packet_slave.cpp"
    # Master packet complete container
    "./extensions/helpers/msgs/packet_controller/Packet_controller.cpp"
)

# Header files
set(PROTOCOL_LOGIC_INCLUDE_DIR 
    # Definitions
    "./logic/definitions/include"
    # Messages store
    "./extensions/helpers/msgs/packet_container/include/"
    # Packet counter
    "./extensions/helpers/msgs/packet_flow/include"
    # Master packet complete container
    "./extensions/helpers/msgs/packet_controller/include"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOCOL_LOGIC_INCLUDE_DIR})

if(${BUILD_TESTING} true)
    message("PROTOCOL LOGIC UNIT TESTS FOUND")

    # Test code
    add_executable(${PROTOCOL_LOGIC_TESTS} 
        # Messages store
        "./extensions/helpers/msgs/packet_container/unit_tests/Mqtt_msg_test.cpp"
        # Packet counter
        "./extensions/helpers/msgs/packet_flow/unit_tests/Packet_flow_test.cpp"
        # Master packet complete container
        "./extensions/helpers/msgs/packet_controller/unit_tests/Packet_controller_test.cpp")

    # Link test library with others
    target_link_libraries(
        ${PROTOCOL_LOGIC_TESTS} 
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${PROTOCOL_LOGIC_TESTS} PROPERTY CXX_STANDARD 17)

    # Find tests
    gtest_discover_tests(${PROTOCOL_LOGIC_TESTS})
endif()

# Commands interpreter console
target_include_directories(${PROJECT_NAME} PUBLIC ${CTRL_CONSOLE_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}  ctrl_console)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)