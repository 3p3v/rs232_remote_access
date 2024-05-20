#pragma once

#include <Port_settings.hpp>
#include <Serial_base.hpp>

namespace Logic
{
    /// @brief Serial settings interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl_t Class implementing functions.
    template <typename Impl_t>
    class Serial_settings : public Serial_base<Impl_t>
    {
        using Serial_base_t::Serial_base;

    public:
        void set_baud_rate(Port_settings::Baud_rate baud_rate);
        void set_parity(Port_settings::Parity parity);
        void set_char_size(Port_settings::Char_size char_size);
        void set_stop_bits(Port_settings::Stop_bits stop_bits);
    };

    // template <typename Impl_t>
    // Serial_settings(Impl_t &&) -> Serial_settings<Impl_t>;

    template <typename Impl_t>
    inline void Serial_settings<Impl_t>::set_baud_rate(Port_settings::Baud_rate baud_rate)
    {
        if (auto i = get_impl())
            i->set_baud_rate(baud_rate);
    }

    template <typename Impl_t>
    inline void Serial_settings<Impl_t>::set_parity(Port_settings::Parity parity)
    {
        if (auto i = get_impl())
            i->set_parity(parity);
    }

    template <typename Impl_t>
    inline void Serial_settings<Impl_t>::set_char_size(Port_settings::Char_size char_size)
    {
        if (auto i = get_impl())
            i->set_char_size(char_size);
    }

    template <typename Impl_t>
    inline void Serial_settings<Impl_t>::set_stop_bits(Port_settings::Stop_bits stop_bits)
    {
        if (auto i = get_impl())
            i->set_stop_bits(stop_bits);
    }
}