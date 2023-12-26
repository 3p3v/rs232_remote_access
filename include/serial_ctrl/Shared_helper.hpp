#pragma once

#include <atomic>
#include <Device.hpp>
#include <Packet_controller.hpp>
#include <Serial_info.hpp>
#include <Packet_defs.hpp>

/// @brief Common objects for communication in both ways
class Shared_helper : protected Packet_defs
{
    using Messages = Logic::Packet_controller<
        char,
        min_msg_num,
        max_msg_num,
        max_saved>;

public:
    /* Storage for sending messages */
    Messages msgs{};
    /* Message counter for received messages */
    Logic::Packet_slave counter{};

    /* Lock communictaion from MQTT to serial */
    std::atomic_bool mqtt_lock{true};
    /* Lock communictaion from serial to MQTT */
    std::atomic_bool phy_lock{true};
};
