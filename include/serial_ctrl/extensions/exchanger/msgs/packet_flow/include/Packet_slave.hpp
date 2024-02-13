#pragma once

#include <Packet_flow.hpp>

/// @brief Packet number control
template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
class Ip_slave
    : public Packet_flow<
          Val_t,
          min_msg_num,
          max_msg_num>
{
    std::atomic_bool s_reload{true};

public:
    /// @brief Set next number as expected
    /// @param num
    /// @return Number expected
    Val_t num_up(Val_t num);

    /// @brief Allow for any number in next turn
    void reload();
};

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
Val_t Ip_slave<
    Val_t,
    min_msg_num,
    max_msg_num>::num_up(Val_t num)
{
    if (s_reload)
    {
        s_reload = false;
        next_num = num + 1;
        return next_num;
    }
    else
    {
        if (next_num != num)
            throw std::logic_error{"Not expected packet number!"};

        return Ip_packet_flow::num_up_();
    }
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num>
void Ip_slave<
    Val_t,
    min_msg_num,
    max_msg_num>::reload()
{
    s_reload = true;
}