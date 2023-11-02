#include <Ip_console.hpp>
#include <Controller.hpp>

Ip_console::Ip_console(Mqtt_port::Controller &controller)
    : controller{controller}
{
}

void Ip_console::leh(const std::string &dev_name, std::string &&data)
{
    controller.write(Device::get_info_ch(dev_name),
                     data.cbegin(),
                     data.cend());
}