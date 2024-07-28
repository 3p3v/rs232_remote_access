#pragma once

template <typename Val_t>
class Packet_val
{
public:
    using UVal_t = std::make_unsigned_t<Val_t>;

    Packet_val();
    Packet_val(Packet_val &&);
    Packet_val &operator=(Packet_val &&);
    Packet_val(const Packet_val &);
    Packet_val &operator=(const Packet_val &);
    virtual ~Packet_val() = 0;
};

template <typename Val_t>
Packet_val<Val_t>::Packet_val() = default;

template <typename Val_t>
Packet_val<Val_t>::Packet_val(Packet_val &&) = default;

template <typename Val_t>
Packet_val<Val_t>& Packet_val<Val_t>::operator=(Packet_val &&) = default;

template <typename Val_t>
Packet_val<Val_t>::Packet_val(const Packet_val &) = default;

template <typename Val_t>
Packet_val<Val_t>& Packet_val<Val_t>::operator=(const Packet_val &) = default;

template <typename Val_t>
Packet_val<Val_t>::~Packet_val() = default;