#pragma once

#include <Serial_base.hpp>

/// @brief Serial settings interface.
///        Declares all methods needed by the functional class to be suted for communication.
/// @tparam Impl Class implementing functions.
template <typename Impl>
class Serial_settings
{
    Impl impl;

public:
    void set_baud_rate(unsigned int baud_rate);
    void set_parity(const Parity parity);
    void set_char_size(unsigned int char_size);
    void set_stop_bits(const Stop_bits stop_bits);

    Serial_settings(Impl &&impl);
};

template <typename Impl>
Serial_settings(Impl &&) -> Serial_settings<Impl>;

template <typename Impl>
inline void Serial_settings<Impl>::set_baud_rate(unsigned int baud_rate)
{
    impl.set_baud_rate(baud_rate);
}

template <typename Impl>
inline void Serial_settings<Impl>::set_parity(const Parity parity)
{
    impl.set_parity(parity);
}

template <typename Impl>
inline void Serial_settings<Impl>::set_char_size(unsigned int char_size)
{
    impl.set_char_size(char_size);
}

template <typename Impl>
inline void Serial_settings<Impl>::set_stop_bits(const Stop_bits stop_bits)
{
    impl.set_stop_bits(stop_bits);
}

template <typename Impl>
inline Serial_settings<Impl>::Serial_settings(Impl &&impl)
    : impl{std::move(impl)}
{
}