#pragma once

namespace Logic
{
    /// @brief MQTT settings interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl Class implementing functions.
    template <typename Impl>
    class Mqtt_settings
    {
        Impl impl;

    public:
        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        Mqtt_settings(Impl &&impl);
    };

    template <typename Impl>
    Mqtt_settings(Impl &&) -> Mqtt_settings<Impl>;

    template <typename Impl>
    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl>::write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write_i(
            std::forward<Cont_t>(msg),
            std::forward<Arg_cont_t>(arg),
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl>::write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write_i(
            std::forward<Cont_t>(msg),
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl>
    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl>::write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write_s(
            std::forward<Cont_t>(msg),
            std::forward<Arg_cont_t>(arg),
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings<Impl>::write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write_s(
            std::forward<Cont_t>(msg),
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    // template <typename Impl>
    // inline Mqtt_settings<Impl>::Mqtt_settings(const Impl &impl)
    //     : Mqtt_settings{impl}
    // {
    // }

    template <typename Impl>
    inline Mqtt_settings<Impl>::Mqtt_settings(Impl &&impl)
        : impl{std::move(impl)}
    {
    }
}