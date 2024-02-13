#pragma once

#include <Packet_flow.hpp>

/// @brief Master packet number control
template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
class Packet_master : public Packet_flow<
                          Val_t,
                          min_msg_num,
                          max_msg_num>
{
public:
    Val_t num_up();
    Val_t num_down();
};

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
Val_t Packet_master<
    Val_t,
    min_msg_num,
    max_msg_num>::num_up()
{
    return num_up_();
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
inline Val_t Packet_master<
    Val_t,
    min_msg_num,
    max_msg_num>::num_down()
{
    return num_down_();
}
