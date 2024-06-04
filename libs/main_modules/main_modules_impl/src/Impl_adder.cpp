#include <Impl_adder.hpp>
#include <Device_impl.hpp>
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
#include <Dev_cont.hpp>

Impl::Impl_adder::Impl_adder(Dev_cont &devs, Impl_cont &infos, Worker_storage &notyfier, Mqtt_controller &controller)
    : Dev_adder{devs, notyfier}, infos{infos}, controller{controller}
{
}

void Impl::Impl_adder::add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info)
{
    std::lock_guard<std::mutex> lock(infos.info_mutex);

    if (devs.num != infos.num)
        throw std::logic_error{"Number of devices and additional information do not match!"};
    
    infos.infos.emplace(infos.num, std::move(info));

    auto &info_ = infos.infos.find(infos.num)->second;

    if (++infos.num == std::numeric_limits<Dev_num>::min())
            throw std::logic_error{"Too many devices connected. Limit reached."};
    
    Dev_adder::add<Device_impl>(
        Remote_dev{std::move(sett), write_access},
        Mqtt_impl{info_, controller},
        Serial_impl{info_});
}
