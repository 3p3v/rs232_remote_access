cmake_minimum_required(VERSION 3.0.0)
project(packet_ctrl VERSION 0.1.0 LANGUAGES CXX)
set(PACKET_CTRL_TESTS packet_ctrl_tests)

# Main code
add_library(${PROJECT_NAME} STATIC
    #
    "./packet_sett/src/Packet_sett_final.cpp"
    # Messages store
    "./packet_container/src/Mqtt_msg_cont_base.cpp"
    "./packet_container/src/Mqtt_msg.cpp"
    "./packet_container/src/Mqtt_msg_cont.cpp"
    # Packet counter
    "./packet_flow/src/Packet_flow.cpp"
    "./packet_flow/src/Packet_master.cpp"
    "./packet_flow/src/Packet_slave.cpp"
    # Master packet complete container
    "./packet_controller/src/Packet_controller.cpp"
)

# Header files
set(PACKET_CTRL_INCLUDE_DIR 
    #
    "./packet_sett/include/"
    # Messages store
    "./packet_container/include/"
    # Packet counter
    "./packet_flow/include"
    # Master packet complete container
    "./packet_controller/include"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PACKET_CTRL_INCLUDE_DIR})

# Add defs lib
target_include_directories(${PROJECT_NAME} PUBLIC ${DEFS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC
    defs
)

if(${BUILD_TESTING} true)
    message("PROTOCOL LOGIC UNIT TESTS FOUND")

    # Test code
    add_executable(${PACKET_CTRL_TESTS} 
        # Messages store
        "./packet_container/unit_tests/Mqtt_msg_test.cpp"
        # Packet counter
        "./packet_flow/unit_tests/Packet_flow_test.cpp"
        # Master packet complete container
        "./packet_controller/unit_tests/Packet_controller_test.cpp")

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
target_link_libraries(${PROJECT_NAME} PUBLIC ctrl_console)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)