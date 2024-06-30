#include <Impl_adder.hpp>
#include <Device_impl.hpp>
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
#include <Dev_cont.hpp>
#include <stdexcept>

Impl::Impl_adder::Impl_adder(Impl_cont &devs, Worker_storage &notyfier, Mqtt_controller &controller)
    : Impl_ext{devs}, notyfier{notyfier}, controller{controller}
{
}

void Impl::Impl_adder::add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info)
{
    std::lock_guard<std::mutex> lock(devs.dev_mutex);

    auto num = devs.num++;

    if (devs.num == std::numeric_limits<Dev_num>::min()) throw std::logic_error{"Too many devices connected. Limit reached."};

    /* Add info and device */
    auto info_ = std::make_shared<Dev_info>(std::move(info));

    devs.devs.emplace(
        num,
        Device_holder::make_device<Device_impl>(
            Notyfier{num, notyfier},
            Remote_dev{std::move(sett), write_access},
            Mqtt_impl{info_, controller},
            Serial_impl{info_}));

    devs.infos.emplace(num, std::move(info_));
}
