#pragma once

/* Protocol implementation */
#include <Device.hpp>
/* Low-level driver classes */
#include <Custom_timer.hpp>
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
/* Full additional phy layer info */
#include <Dev_info.hpp>

using namespace Logic;

namespace Impl
{
    class Device_impl final
        : public Device<
              Custom_timer_maker,
              Mqtt_impl,
              Serial_impl>
    {
        using Device_t = Device<Custom_timer_maker, Mqtt_impl, Serial_impl>;
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    public:
        template <typename Device_ptr_t>
        Device_impl(
            Notyfier &&notyfier,
            Device_ptr_t &&dev,
            Remote_dev &&rec,
            Mqtt_impl &&remote,
            Serial_impl &&serial);
    };

    template <typename Device_ptr_t>
    inline Device_impl::Device_impl(
        Notyfier &&notyfier,
        Device_ptr_t &&dev,
        Remote_dev &&rec,
        Mqtt_impl &&remote,
        Serial_impl &&serial)
        : Device{
              std::move(notyfier),
              std::forward<Device_ptr_t>(dev),
              std::move(rec),
              std::move(remote),
              std::move(serial)}
    {
    }
}