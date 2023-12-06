#pragma once

#include <Ip_packet_flow.hpp>

namespace Ip_serial
{
    /// @brief Packet number control
    class Ip_slave : public Ip_packet_flow
    {
        std::atomic_bool s_reload{true};

    public:
        /// @brief Set next number as expected
        /// @param num 
        /// @return Number expected
        char num_up(char num);

        /// @brief Allow for any number in next turn
        void reload();
    };
}