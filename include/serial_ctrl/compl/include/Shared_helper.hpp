#pragma once

#include <memory>
#include <atomic>
#include <Device.hpp>
#include <Packet_controller.hpp>
#include <Serial_info.hpp>
#include <Packet_defs.hpp>

namespace Logic
{
    class Under_exchanger;

    class Under_setter;

    /// @brief Common objects for communication in both ways
    class 
    Shared_helper : protected Packet_defs
    {
        using Messages = Logic::Packet_controller<
            char,
            min_msg_num,
            max_msg_num,
            max_saved>;

    public:
        /* Data handling */
        std::weak_ptr<Under_exchanger> exchanger{nullptr};
        /* Settings handling */
        std::weak_ptr<Under_setter> setter{nullptr};

        /* Storage for sending messages */
        Messages msgs{};
        /* Message counter for received messages */
        Packet_slave counter{};

        /* Lock communictaion from MQTT to serial */
        std::atomic_bool mqtt_lock{true};
        /* Lock communictaion from serial to MQTT */
        std::atomic_bool phy_lock{true};
    };
}
