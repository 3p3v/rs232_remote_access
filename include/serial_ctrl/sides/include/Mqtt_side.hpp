#pragma once

#include <Mqtt_base.hpp>

namespace Logic
{
    /// @brief MQTT data interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl_t Class implementing functions.
    template <typename Impl_t>
    class Mqtt_side : public Mqtt_base<Impl_t>
    {
        using Mqtt_base_t::Mqtt_base;

    public:
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
    };

    // template <typename Impl_t>
    // Mqtt_side(Impl_t &&) -> Mqtt_side<Impl_t>;

    template <typename Impl_t>
    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_side<Impl_t>::write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->write(
            id,
            begin,
            end,
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }
}