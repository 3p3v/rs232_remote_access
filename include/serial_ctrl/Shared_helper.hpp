#pragma once

#include <atomic>
#include <Device.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Ip_master.hpp>
#include <Serial_info.hpp>

/// @brief Common objects for communication in both ways
class Shared_helper
{
public:
    /* Storage for sending messages */
    Ip_serial::Mqtt_msg_cont msgs{}; 
    /* Message counting in respect of protocol */
    Ip_serial::Ip_master master_counter{};  

    /* Lock communictaion from MQTT to serial */
    std::atomic_bool mqtt_lock{true};
    /* Lock communictaion from serial to MQTT */
    std::atomic_bool phy_lock{true};
};

