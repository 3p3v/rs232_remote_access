#include <Impl_adder.hpp>
#include <Device_impl.hpp>
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
#include <Dev_cont.hpp>

Impl::Impl_adder::Impl_adder(Dev_cont &devs, Impl_cont &infos, Worker_storage &notyfier, Mqtt_controller &controller)
    : Dev_adder{devs, notyfier}, infos{infos}, controller{controller}
{
}

void Impl::Impl_adder::add(Remote_dev &&rec, Dev_info &&info)
{
    std::lock_guard<std::mutex> lock(infos.info_mutex);

    if (devs.num != infos.num)
        throw std::logic_error{"Number of devices and additional information do not match!"};
    
    infos.infos.emplace(infos.num, std::move(info));

    if (++infos.num == std::numeric_limits<Dev_num>::min())
            throw std::logic_error{"Too many devices connected. Limit reached."};
    
    Dev_adder::add<Device_impl>(
        std::move(rec),
        Mqtt_impl{info, controller},
        Serial_impl{info});
}
