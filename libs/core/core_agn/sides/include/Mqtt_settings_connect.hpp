#pragma once

#include <Mqtt_base.hpp>

namespace Logic
{
    /// @brief MQTT connection start settings interface.
    /// @tparam Impl_t Class implementing functions.
    template <typename Impl_t>
    class Mqtt_settings_connect : public Mqtt_base<Impl_t>
    {
        using Mqtt_base_t::Mqtt_base;

    public:
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);
    };

    // template <typename Impl_t>
    // Mqtt_settings_connect(Impl_t &&) -> Mqtt_settings_connect<Impl_t>;

    template <typename Impl_t>
    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_settings_connect<Impl_t>::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->connect_i(
                std::forward<Ok_callb>(ok_callb),
                std::forward<Ec_callb>(ec_callb));
    }
}