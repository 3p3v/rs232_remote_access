#pragma once

#include <Intf_base.hpp>

namespace Logic
{
    /// @brief Base class for all MQTT related components
    /// @tparam Impl_t 
    template <typename Impl_t>
    class Mqtt_base : public Intf_base<Impl_t>
    {
    protected:
        using Mqtt_base_t = Mqtt_base<Impl_t>;

    private:
        using Intf_base_t::Intf_base;
    };
}