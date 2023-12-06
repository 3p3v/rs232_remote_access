#pragma once

#include <atomic>
#include <Ip_packet.hpp>

namespace Ip_serial
{
    /// @brief Packet number control
    class Ip_packet_flow : protected Ip_packet
    {
    protected:
        std::atomic<char> next_num{min_msg_num};
        std::atomic<unsigned char> not_acked{0};

    public:
        /// @brief Increment message within range 
        /// @param next_n 
        /// @return Previous value
        char num_up();

        unsigned char get_not_acked();
        void ack(char id);

        char exp();

    public:
        Ip_packet_flow() = default;
        Ip_packet_flow(Ip_packet_flow&&) = default;
        Ip_packet_flow& operator=(Ip_packet_flow&&) = default;
        Ip_packet_flow(const Ip_packet_flow&) = default;
        Ip_packet_flow& operator=(const Ip_packet_flow&) = default;
        virtual ~Ip_packet_flow() = 0;
    };
}