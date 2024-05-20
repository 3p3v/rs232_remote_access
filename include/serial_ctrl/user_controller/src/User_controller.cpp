#include <User_controller.hpp>
/* Jobs */
#include <Change_param_job.hpp>
#include <Start_job.hpp>
#include <Restart_job.hpp>
/* Message forwarding */
#include <Forwarder.hpp>

using namespace Logic::Port_settings;

namespace Logic
{
    // void User_controller::set_baud_rate(Port_settings::Baud_rate baud_rate)
    // {
    //     auto s = settings->port_settings;
    //     s.baud_rate = baud_rate;

    //     manager->forward_job(Change_param_job{std::move(s)});
    // }

    // void User_controller::set_parity(Port_settings::Parity parity)
    // {
    //     auto s = settings->port_settings;
    //     s.parity = parity;

    //     manager->forward_job(Change_param_job{std::move(s)});
    // }

    // void User_controller::set_char_size(Port_settings::Char_size char_size)
    // {
    //     auto s = settings->port_settings;
    //     s.char_size = char_size;

    //     manager->forward_job(Change_param_job{std::move(s)});
    // }

    // void User_controller::set_stop_bits(Port_settings::Stop_bits stop_bits)
    // {
    //     auto s = settings->port_settings;
    //     s.stop_bits = stop_bits;

    //     manager->forward_job(Change_param_job{std::move(s)});
    // }

    void User_controller::set_params(Port_settings_storage &&port_settings)
    {
        manager.forward_job(Change_param_job{std::move(port_settings)});
    }

    void User_controller::set_params(const Port_settings_storage &port_settings)
    {
        manager.forward_job(Change_param_job{port_settings});
    }

    void User_controller::run()
    {
        manager.forward_job(Start_job{});
    }

    void User_controller::restart()
    {
        manager.forward_job(Restart_job{});
    }

    User_controller::User_controller(Forwarder &&manager)
        : manager{std::move(manager)}
    {
    }

    User_controller::User_controller(const Forwarder &manager)
        : manager{manager}
    {
    }
}
