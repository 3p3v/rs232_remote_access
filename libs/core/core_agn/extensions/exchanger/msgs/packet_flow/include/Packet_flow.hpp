#pragma once

#include <atomic>
#include <Packet_sett.hpp>
#include <Packet_val.hpp>

/// @brief Packet number control
template <typename Val_t>
class Packet_flow : public Packet_val<Val_t>
{
protected:
    const Packet_sett<Val_t> &ps;

    Val_t next_num{ps.min_msg_num};
    UVal_t not_acked{0};

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

    Packet_flow(const Packet_sett<Val_t> &ps);
    Packet_flow(Packet_flow &&) noexcept;
    Packet_flow &operator=(Packet_flow &&) = delete;
    Packet_flow(const Packet_flow &) = delete;
    Packet_flow &operator=(const Packet_flow &) = delete;
    virtual ~Packet_flow() = 0;
};

template <
    typename Val_t>
Val_t Packet_flow<
    Val_t>::num_up_()
{
    Val_t temp = next_num;

    /* Increment not acked */
    not_acked++;

    /*  */
    if (++next_num > ps.max_msg_num)
    {
        next_num = ps.min_msg_num;
    }

    return temp;
}

template <
    typename Val_t>
inline Val_t Packet_flow<
    Val_t>::num_down_()
{
    if (--next_num < ps.min_msg_num)
        next_num = ps.max_msg_num;

    return next_num;
}

template <
    typename Val_t>
std::make_unsigned_t<Val_t> Packet_flow<
    Val_t>::get_not_acked()
{
    return not_acked;
}

template <
    typename Val_t>
void Packet_flow<
    Val_t>::ack(Val_t id)
{
    not_acked = next_num - id - 1;
}

template <
    typename Val_t>
Val_t Packet_flow<
    Val_t>::exp()
{
    return next_num;
}

template <
    typename Val_t>
inline Packet_flow<Val_t>::Packet_flow(const Packet_sett<Val_t> &ps)
    : ps{ps}
{
}

template <typename Val_t>
inline Packet_flow<Val_t>::~Packet_flow() = default;

template <
    typename Val_t>
inline Packet_flow<Val_t>::Packet_flow(Packet_flow &&pf) noexcept
    : ps{pf.ps},
      next_num{pf.next_num},
      not_acked{pf.not_acked}
{
}
