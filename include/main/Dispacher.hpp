#pragma once

#include <memory>
#include <atomic>
#include <Server.hpp>
#include <User.hpp>

class Dispacher_impl;

namespace Main_serial
{
    class Controller;
    class Monitor;
}

class Dispacher
{
    Dispacher();
    
    static Dispacher dispacher_s;

    std::atomic_bool credential_supplied{false};
    Mqtt_port::Server::Get_cont server;
    Mqtt_port::User::Get_cont user;
    
    std::atomic_bool reboot_lock{false};
    std::unique_ptr<Dispacher_impl> dispacher{nullptr};

public:
    static Main_serial::Controller &get_controller();

    static Main_serial::Controller &get_controller(Mqtt_port::Server::Get_cont &&server,
                                                   Mqtt_port::User::Get_cont &&user);
    static Main_serial::Monitor &get();

    static void reboot();
};

using Monitor = Dispacher;
