#pragma once

#include <Mqtt_msg.hpp>
#include <Ip_packet.hpp>

namespace Ip_serial
{
    class Console;

    class Mqtt_msg_cont : protected Ip_packet
    {
        std::array<Mqtt_msg, max_saved> msgs;

    public:
        /// @brief Get msg with given id, if not found throws exception
        /// @param id 
        /// @return 
        Mqtt_msg& operator[](unsigned char id);

        /// @brief Find first free message
        /// @return 
        Mqtt_msg& first_free();

        Mqtt_msg& oldest();

        /// @brief Free all messages that have id < argument
        /// @param id 
        void free_untill(unsigned char id);

        void unmark(unsigned char id);
    };
}