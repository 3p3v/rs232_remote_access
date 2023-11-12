#pragma once

#include <unordered_map>
#include <memory>
#include <Main_defs.hpp>
#include <Server.hpp>
#include <User.hpp>

namespace Main_serial
{
    class Controller;
    class Monitor;
}

class Dispacher_impl
{
    std::unique_ptr<Main_serial::Controller> controller;
    std::unique_ptr<Main_serial::Monitor> monitor;
    std::unordered_map<Main_serial::Device_ptr, Main_serial::Serial_pair> devices;
public:
    Main_serial::Controller &get_controller();
    Main_serial::Monitor &get_monitor();

    Dispacher_impl(Mqtt_port::Server::Get_cont server, Mqtt_port::User::Get_cont user);
    ~Dispacher_impl();
};
