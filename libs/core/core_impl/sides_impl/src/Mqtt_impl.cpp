#include <Mqtt_impl.hpp>

namespace Impl
{
    Mqtt_impl::Mqtt_impl(Remote_rec &info, Mqtt_controller &controller)
        : info{info}, controller{controller}
    {
    }

    Mqtt_impl::Mqtt_impl(Mqtt_impl &&mi) noexcept
        : info{std::move(mi.info)}, controller{mi.controller}
    {
        mi.moved = true;
    }

    Mqtt_impl::~Mqtt_impl()
    {
        /* Do not call unsubscribe if destructed due to move operation */
        if (!moved)
        {
            controller.unsubscribe(info.data_ch);
            controller.unsubscribe(info.info_ch);
        }
    }

    Mqtt_impl_factory::Mqtt_impl_factory(Mqtt_controller &controller)
        : controller{controller}
    {
    }

    Mqtt_impl Mqtt_impl_factory::create(Remote_rec &info)
    {
        return Mqtt_impl{info, controller};
    }
}