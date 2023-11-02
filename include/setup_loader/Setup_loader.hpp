#pragma once

#include <iostream>
#include <string>
#include <User.hpp>
#include <Server.hpp>
#include <Ctrl_console.hpp>
#include <Device.hpp>
#include <Setup_console.hpp>

using namespace Cmd_ctrl;
using namespace Cmd_ctrl::Setup;

class Setup_loader final
{
    using Handle = Setup::Handle_t;
    using Handle_type = Setup::Handle_t::Base_handle_intf::Type;

public:
    class Data_pack
    {
    public:
        Mqtt_port::User::Cont user{};
        Mqtt_port::Server::Cont server{};
        Device device;
    };

private:
    std::string file_name;
    Setup_console console{};
    Data_pack data_pack;
    bool data_loaded{false};

    void load_data_handler();

public:
    template <typename Str>
    Setup_loader(Str &&file_name);

    Data_pack &load_data() &;
    Data_pack load_data() &&;

    Data_pack &get_data_pack() &;
};

template <typename Str>
Setup_loader::Setup_loader(Str &&file_name)
    : file_name{std::forward<Str>(file_name)}
{
    console.add_cmd("username", Handle::Policies<>::Dyn_handle(
                                    [this](std::string &&str)
                                    {
                                        data_pack.user.emplace(Mqtt_port::User::Option::username, std::move(str));
                                    }));

    console.add_cmd("password", Handle::Policies<>::Dyn_handle(
                                    [this](std::string &&str)
                                    {
                                        data_pack.user.emplace(Mqtt_port::User::Option::password, std::move(str));
                                    }));

    console.add_cmd("user_id", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       data_pack.user.emplace(Mqtt_port::User::Option::id, std::move(str));
                                       data_pack.user.emplace(Mqtt_port::User::Option::no_clean, "");
                                   }));

    auto handle = Handle::Policies<>::Dyn_handle{[this](std::string &&str)
                                                 {
                                                     data_pack.server.emplace(Mqtt_port::Server::Option::ip, std::move(str));
                                                 }};
    handle.set_mandatority(Handle_type::mandatory);
    console.add_cmd("ip", std::move(handle));

    auto handle1 = Handle::Policies<>::Dyn_handle{[this](std::string &&str)
                                            {
                                                data_pack.server.emplace(Mqtt_port::Server::Option::port, std::move(str));
                                            }};
    handle1.set_mandatority(Handle_type::mandatory);
    console.add_cmd("port", std::move(handle1));

    auto handle2 = Handle::Policies<>::Dyn_handle{[this](std::string &&str)
                                            {
                                                data_pack.server.emplace(Mqtt_port::Server::Option::pem, std::move(str));
                                            }};
    handle2.set_mandatority(Handle_type::mandatory);
    console.add_cmd("pem", std::move(handle2));

    auto handle3 = Handle::Policies<>::Dyn_handle{[this](std::string &&str)
                                            {
                                                data_pack.server.emplace(Mqtt_port::Server::Option::cert, std::move(str));
                                            }};
    handle3.set_mandatority(Handle_type::mandatory);
    console.add_cmd("crt", std::move(handle3));

    auto handle4 = Handle::Policies<>::Dyn_handle{[this](std::string &&str)
                                            {
                                                data_pack.device.set_name(std::move(str));
                                            }};
    handle4.set_mandatority(Handle_type::mandatory);
    console.add_cmd("device", std::move(handle4));
}
