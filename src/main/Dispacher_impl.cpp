#include <Dispacher_impl.hpp>
#include <Monitor.hpp>
#include <main/Controller.hpp>

Dispacher_impl::Dispacher_impl(Mqtt_port::Server::Get_cont server, Mqtt_port::User::Get_cont user)
    : devices{},
      controller{std::make_unique<Main_serial::Controller>(std::move(server), std::move(user), devices)},
      monitor{std::make_unique<Main_serial::Monitor>(*controller, devices)}
{
}

Dispacher_impl::~Dispacher_impl()
{
    monitor.reset(nullptr);
    controller.reset(nullptr);
}

Main_serial::Controller &Dispacher_impl::get_controller()
{
    return *controller;
}

Main_serial::Monitor &Dispacher_impl::get_monitor()
{
    return *monitor;
}