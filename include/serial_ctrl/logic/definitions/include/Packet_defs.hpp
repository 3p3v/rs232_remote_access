#pragma once

#include <string_view>

class Packet_defs
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

    Packet_defs() = default;
    Packet_defs(Packet_defs &&) = default;
    Packet_defs &operator=(Packet_defs &&) = default;
    Packet_defs(const Packet_defs &) = default;
    Packet_defs &operator=(const Packet_defs &) = default;
    virtual ~Packet_defs() = 0;
};
