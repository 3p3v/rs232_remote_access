#pragma once

#include <Mqtt_base.hpp>

namespace Logic
{
    /// @brief MQTT settings interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl_t Class implementing functions.
    template <typename Impl_t>
    class Mqtt_settings : public Mqtt_base<Impl_t>
    {
        using Mqtt_base_t::Mqtt_base;

    public:
        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
    };

    // template <typename Impl_t>
    // Mqtt_settings(Impl_t &&) -> Mqtt_settings<Impl_t>;

    template <typename Impl_t>
    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl_t>::write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->write_i(
                std::forward<Cont_t>(msg),
                std::forward<Arg_cont_t>(arg),
                std::forward<Ok_callb>(ok_callb),
                std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl_t>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl_t>::write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->write_i(
                std::forward<Cont_t>(msg),
                std::forward<Ok_callb>(ok_callb),
                std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl_t>
    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl_t>::write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->write_s(
                std::forward<Cont_t>(msg),
                std::forward<Arg_cont_t>(arg),
                std::forward<Ok_callb>(ok_callb),
                std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl_t>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl_t>::write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->write_s(
                std::forward<Cont_t>(msg),
                std::forward<Ok_callb>(ok_callb),
                std::forward<Ec_callb>(ec_callb));
    }
}