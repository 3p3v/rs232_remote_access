#pragma once

#include <Ctrl_console.hpp>
#include <Device.hpp>
#include <Controller.hpp>

class Ip_console final : public Cmd_ctrl::Ctrl::Ctrl_console
{
    Mqtt_port::Controller &controller;

protected:
    void leh(const std::string &dev_name, std::string &&data) override;

public:
    Ip_console(Mqtt_port::Controller &controller);
};
