#pragma once

#include <string_view>

namespace Ip_serial
{
    class Ip_packet
    {
    public:
        static constexpr char min_msg_num{' '};
        static constexpr char max_msg_num{'~'};

        static constexpr std::string_view packet_ack_s{"A"};
        static constexpr std::string_view packet_num_s{"P"};

        static constexpr std::string_view invalid_number_s{"I_N"};
        static constexpr std::string_view no_number_s{"N_N"};

        static constexpr unsigned char ack_after{8};
        static constexpr unsigned char max_saved{30};
        static constexpr unsigned char max_not_ack{16};

        Ip_packet() = default;
        Ip_packet(Ip_packet &&) = default;
        Ip_packet &operator=(Ip_packet &&) = default;
        Ip_packet(const Ip_packet &) = default;
        Ip_packet &operator=(const Ip_packet &) = default;
        virtual ~Ip_packet() = 0;
    };
}