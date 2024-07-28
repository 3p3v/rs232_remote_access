#pragma once

#include <Packet_flow.hpp>

/// @brief Packet number control
template <typename Val_t>
class Packet_slave
    : public Packet_flow<Val_t>
{
    using Packet_flow<Val_t>::Packet_flow;

    bool s_reload{true};

public:
    /// @brief Set next number as expected
    /// @param num
    /// @return Number expected
    Val_t num_up(Val_t num);

    /// @brief Allow for any number in next turn
    void reload() noexcept;
};

template <typename Val_t>
Val_t Packet_slave<Val_t>::num_up(Val_t num)
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

        return Packet_flow::num_up_();
    }
}

template <typename Val_t>
void Packet_slave<Val_t>::reload() noexcept
{
    s_reload = true;
}
