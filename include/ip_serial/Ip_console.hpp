#pragma once

#include <Ctrl_console.hpp>
#include <Device.hpp>

class Ip_console final : public Cmd_ctrl::Ctrl::Ctrl_console
{
    Mqtt_port::Controller &controller;

protected:
    void leh(const std::string &dev_name, std::string &&data) override;

public:
    Ip_console(Mqtt_port::Controller &controller);
};

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