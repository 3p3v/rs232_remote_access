#pragma once

#include <Packet_flow.hpp>

/// @brief Master packet number control
template <typename Val_t>
class Packet_master
    : public Packet_flow<Val_t>
{
    using Packet_flow<Val_t>::Packet_flow;

public:
    Val_t num_up();
    Val_t num_down();
    void reload() noexcept;
};

template <
    typename Val_t>
Val_t Packet_master<
    Val_t>::num_up()
{
    return num_up_();
}

template <
    typename Val_t>
inline Val_t Packet_master<
    Val_t>::num_down()
{
    return num_down_();
}

template <
    typename Val_t>
inline void Packet_master<
    Val_t>::reload() noexcept
{
    // next_num = min_msg_num; // TODO
}
