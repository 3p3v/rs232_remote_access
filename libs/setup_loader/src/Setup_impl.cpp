#include <Setup_impl.hpp>
#include <iostream>
#include <fstream>
#include <Set_defs.hpp>
#include <Setup_defs.hpp>
/* Policies */
#include <Numbers_only.hpp>

namespace Impl
{
    Setup_impl::Setup_impl()
    {
        /* Connection settings */
        console.add_cmd(
            "username",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.user.emplace(Mqtt_port::User::Option::id, str);
                    data_pack.user.emplace(Mqtt_port::User::Option::username, std::move(str));
                }));

        console.add_cmd(
            "password",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.user.emplace(Mqtt_port::User::Option::password, std::move(str));
                }));

        console.add_cmd(
            "ip",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::ip, std::move(str));
                }});

        console.add_cmd(
            "port",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::port, std::move(str));
                }});

        console.add_cmd(
            "pem",
            // Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::pem, std::move(str));
                }});

        console.add_cmd(
            "cert",
            // Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    data_pack.server.emplace(Mqtt_port::Server::Option::cert, std::move(str));
                }});

        /* Device setings */
        console.add_cmd(
            "device",
            Mandatoriness::Mandatory,
            Command::Policies<>::Dyn_handle{
                [this](std::string &&str)
                {
                    auto itr = std::find(
                        data_pack.devices.begin(),
                        data_pack.devices.end(),
                        str);

                    if (itr == data_pack.devices.end())
                    {
                        data_pack.devices.emplace_back<Info_pack>(std::move(str));
                        current_ = data_pack.devices.end() - 1;
                    }
                    else
                    {
                        current_ = itr;
                    }
                }});

        console.add_cmd(
            "baud_rate",
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.baud_rate = Set_defs::baud_rate_trans(str);
                }));

        console.add_cmd(
            "parity",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.parity = Set_defs::parity_trans(str);
                }));

        console.add_cmd(
            "char_size",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.char_size = Set_defs::char_size_trans(str);
                }));

        console.add_cmd(
            "stop_bits",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.sett.stop_bits = Set_defs::stop_bits_trans(str);
                }));

        console.add_cmd(
            "write_access",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.write_access = Setup_defs::write_access_trans(str);
                }));

        console.add_cmd(
            "com",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    auto &current_device = get_current();
                    current_device.info.port = std::move(str);
                    current_device.port_set = true;             // TODO remove once port is set in same line with MAC
                }));

        /* App settings */
        console.add_cmd(
            "debug",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.debug = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_timeout",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_timeout = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_data_loss",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_data_loss = Setup_defs::bool_trans(str);
                }));

        console.add_cmd(
            "close_on_protocol_error",
            Command::Policies<>::Dyn_handle(
                [this](std::string &&str)
                {
                    data_pack.app_opts.close_on_protocol_error = Setup_defs::bool_trans(str);
                }));
    }

    Setup_impl::Info_pack &Setup_impl::get_current()
    {
        if (current_ == data_pack.devices.end())
            throw std::logic_error{"No device supplied! Cannot continue operation!"};

        return *current_;
    }
}