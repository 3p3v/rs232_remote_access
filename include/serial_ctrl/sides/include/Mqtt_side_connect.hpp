#pragma once

namespace Logic
{
    /// @brief MQTT connection start settings interface.
    /// @tparam Impl Class implementing functions.
    template <typename Impl>
    class Mqtt_side_connect
    {
        Impl impl;

    public:
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        Mqtt_side_connect(Impl &&impl);
    };

    template <typename Impl>
    Mqtt_side_connect(Impl &&) -> Mqtt_side_connect<Impl>;

    template <typename Impl>
    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_side_connect<Impl>::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.connect(
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    // template <typename Impl>
    // inline Mqtt_settings<Impl>::Mqtt_settings(const Impl &impl)
    //     : Mqtt_settings{impl}
    // {
    // }

    template <typename Impl>
    inline Mqtt_side_connect<Impl>::Mqtt_side_connect(Impl &&impl)
        : impl{std::move(impl)}
    {
    }
}