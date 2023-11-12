#include <Dispacher.hpp>
#include <stdexcept>
#include <Dispacher_impl.hpp>

Dispacher Dispacher::dispacher_s{};

Dispacher::Dispacher()
{

}

Main_serial::Controller &Dispacher::get_controller()
{
    while (dispacher_s.reboot_lock);
    
    if (dispacher_s.credential_supplied)
    {
        return dispacher_s.dispacher->get_controller();
    }
    else
    {
        throw std::logic_error{"Can not execute action while credentials were not supplied."};
    }
}

Main_serial::Controller &Dispacher::get_controller(Mqtt_port::Server::Get_cont &&server,
                                                   Mqtt_port::User::Get_cont &&user)
{
    while (dispacher_s.reboot_lock);

    /* Lock */
    dispacher_s.reboot_lock = true;

    dispacher_s.server = std::move(server);
    dispacher_s.user = std::move(user);
    dispacher_s.dispacher.reset(new Dispacher_impl{dispacher_s.server, dispacher_s.user});
    dispacher_s.credential_supplied = true;

    /* Unlock */
    dispacher_s.reboot_lock = false;

    return dispacher_s.dispacher->get_controller();
}

Main_serial::Monitor &Dispacher::get()
{
    while (dispacher_s.reboot_lock);

    if (dispacher_s.credential_supplied)
    {
        return dispacher_s.dispacher->get_monitor();
    }
    else
    {
        throw std::logic_error{"Can not execute action while credentials were not supplied."};
    }
}

void Dispacher::reboot()
{
    while (dispacher_s.reboot_lock);

    if (dispacher_s.credential_supplied)
    {
        /* Lock */
        dispacher_s.reboot_lock = true;

        /* Reset */
        dispacher_s.dispacher.reset(new Dispacher_impl{dispacher_s.server, dispacher_s.user});

        /* Unlock */
        dispacher_s.reboot_lock = false;
    }
    else
    {
        throw std::logic_error{"Can not execute action while credentials were not supplied."};
    }
}
