#pragma once

#include <atomic>

/// @brief Packet number control
template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename = typename std::enable_if_t<
        std::is_same_v<Val_t, std::decay_t<Val_t>>>,
    typename = typename std::enable_if_t<
        (min_msg_num < max_msg_num)>>
class Packet_flow
{
protected:
    std::atomic<Val_t> next_num{min_msg_num};
    std::atomic<std::make_unsigned_t<Val_t>> not_acked{0};

    /// @brief Increment message within range
    /// @param next_n
    /// @return Previous value
    Val_t num_up_();

    Val_t num_down_();

public:
    /// @brief How many messages were not acked
    /// @return
    std::make_unsigned_t<Val_t> get_not_acked();

    /// @brief Ack untill
    /// @param id
    void ack(Val_t id);

    /// @brief Expected number
    /// @return
    Val_t exp();

    Packet_flow() = default;
    Packet_flow(Packet_flow &&) = default;
    Packet_flow &operator=(Packet_flow &&) = default;
    Packet_flow(const Packet_flow &) = default;
    Packet_flow &operator=(const Packet_flow &) = default;
    virtual ~Packet_flow() = default;
};

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename T1,
    typename T2>
Val_t Packet_flow<
    Val_t,
    min_msg_num,
    max_msg_num,
    T1,
    T2>::num_up_()
{
    Val_t temp = next_num;

    /* Increment not acked */
    not_acked++;

    /*  */
    if (++next_num > max_msg_num)
    {
        next_num = min_msg_num;
    }

    return temp;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename T1,
    typename T2>
inline Val_t Packet_flow<
    Val_t,
    min_msg_num,
    max_msg_num,
    T1,
    T2>::num_down_()
{
    if (--next_num < min_msg_num)
        next_num = max_msg_num;
    
    return next_num;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename T1,
    typename T2>
std::make_unsigned_t<Val_t> Packet_flow<
    Val_t,
    min_msg_num,
    max_msg_num,
    T1,
    T2>::get_not_acked()
{
    return not_acked;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename T1,
    typename T2>
void Packet_flow<
    Val_t,
    min_msg_num,
    max_msg_num,
    T1,
    T2>::ack(Val_t id)
{
    not_acked = next_num - id - 1;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    typename T1,
    typename T2>
Val_t Packet_flow<
    Val_t,
    min_msg_num,
    max_msg_num,
    T1,
    T2>::exp()
{
    return next_num;
}