cmake_minimum_required(VERSION 3.0.0)
project(protocol_logic VERSION 0.1.0 LANGUAGES CXX)
set(TEST_NAME protocol_logic_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    # Definitions
    "./logic/definitions/Packet_defs.cpp"
    # Messages store
    "./logic/msgs/Mqtt_msg_cont_base.cpp"
    "./logic/msgs/Mqtt_msg.cpp"
    "./logic/msgs/Mqtt_msg_cont.cpp"
    # Packet counter
    "./logic/packet_flow/Packet_flow.cpp"
    "./logic/packet_flow/Packet_master.cpp"
    "./logic/packet_flow/Packet_slave.cpp"
)

# Header files
set(PROTOCOL_LOGIC_INCLUDE_DIR 
    # Definitions
    "./logic/definitions/include"
    # Messages store
    "./logic/msgs/include/"
    # Packet counter
    "./logic/packet_flow/include"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOCOL_LOGIC_INCLUDE_DIR})

if(${BUILD_TESTING} true)
    message("PROTOCOL LOGIC UNIT TESTS FOUND")

    # Test code
    add_executable(${TEST_NAME} 
        # Messages store
        "./logic/msgs/unit_tests/Mqtt_msg_test.cpp"
        # Packet counter
        "./logic/packet_flow/unit_tests/Packet_flow_test.cpp")

    # Link test library with others
    target_link_libraries(
        ${TEST_NAME} #PUBLIC
        ${PROJECT_NAME}
        GTest::gtest_main
    )

    set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD 17)

    # target_include_directories(${TEST_NAME} PUBLIC ${PROTOCOL_LOGIC_INCLUDE_DIR})

    # Find tests
    gtest_discover_tests(${TEST_NAME})
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)