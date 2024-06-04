#include <Mqtt_impl.hpp>

namespace Impl
{
    Mqtt_impl::Mqtt_impl(Remote_info &info, Mqtt_controller &controller)
        : info{info}, controller{controller}
    {
    }

    Mqtt_impl::Mqtt_impl(Mqtt_impl &&mi) noexcept
        : info{std::move(mi.info)}, controller{mi.controller}
    {
        mi.moved = true;
    }

    // Mqtt_impl& Mqtt_impl::operator=(Mqtt_impl &&mi) noexcept
    // {
    //     info = std::move(mi.info);
    //     controller = mi.controller;
    //     moved = false;
        
    //     mi.moved = true;

    //     return *this;
    // }

    Mqtt_impl::~Mqtt_impl()
    {
        if (!moved)
        {
            controller.unsubscribe(info.data_ch);
            controller.unsubscribe(info.info_ch);
        }
    }
}