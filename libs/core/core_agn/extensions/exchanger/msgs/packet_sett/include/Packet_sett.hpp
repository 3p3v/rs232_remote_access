#pragma once

#include <type_traits>

template <
    typename Val_t_,
    typename = typename std::enable_if_t<
        std::is_same_v<Val_t_, std::decay_t<Val_t_>>>>
class Packet_sett
{
public:
    using Val_t = Val_t_;
    using UVal_t = std::make_unsigned_t<Val_t>;

    const Val_t min_msg_num;
    const Val_t max_msg_num;
    const UVal_t max_saved;

    Packet_sett(const Val_t min_msg_num, const Val_t max_msg_num, const UVal_t max_saved);
    Packet_sett(Packet_sett &&) noexcept;
    Packet_sett &operator=(Packet_sett &&) noexcept;
    Packet_sett(const Packet_sett &) noexcept;
    Packet_sett &operator=(const Packet_sett &) noexcept;
    virtual ~Packet_sett();
};

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1>::Packet_sett(const Val_t min_msg_num, const Val_t max_msg_num, const UVal_t max_saved)
    : min_msg_num{min_msg_num}, max_msg_num{max_msg_num}, max_saved{max_saved}
{
}

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1>::Packet_sett(Packet_sett &&) noexcept = default;

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1> &Packet_sett<Val_t_, T1>::operator=(Packet_sett &&) noexcept = default;

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1>::Packet_sett(const Packet_sett &) noexcept = default;

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1> &Packet_sett<Val_t_, T1>::operator=(const Packet_sett &) noexcept = default;

template <typename Val_t_, typename T1>
inline Packet_sett<Val_t_, T1>::~Packet_sett() = default;
