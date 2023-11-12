#pragma once

#include <iostream>
#include <string>
#include <User.hpp>
#include <Server.hpp>
#include <Ctrl_console.hpp>
#include <Device.hpp>
#include <Setup_console.hpp>
#include <Serial_info.hpp>
#include <Ip_defs.hpp>
#include <Setup_except.hpp>
#include <Serial_defs.hpp>
/* Policies */
#include <Numbers_only.hpp>

using namespace Cmd_ctrl;
using namespace Cmd_ctrl::Setup;

class Setup_loader final : protected Ip_serial::Ip_defs, protected Serial_defs
{
public:
    class Info_pack
    {
    public:
        bool write_access{false};
        bool settings_known{false}; 
        Serial_info info;
    };

private:
    using Handle = Setup::Handle_t;
    using Handle_type = Setup::Handle_t::Base_handle_intf::Type;

    using Device_pair = std::pair<Device, Info_pack>;
    using Device_cont = std::vector<Device_pair>;
    using Device_iter = Device_cont::iterator;

public:
    class Data_pack
    {
    public:
        bool close_on_timeout{true};
        bool auto_reload{true};
        bool debug{false};
        Mqtt_port::User::Cont user{};
        Mqtt_port::Server::Cont server{};
        Device_cont devices;
    };

private:
    std::string file_name;
    Setup_console console{};
    Data_pack data_pack;
    bool data_loaded{false};

    Device_iter current_{data_pack.devices.end()};
    Device_iter &get_current();

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
                                                      data_pack.devices.emplace_back(std::make_pair(Device{std::move(str)}, Info_pack{}));
                                                      current_ = data_pack.devices.end() - 1;
                                                  }};
    handle4.set_mandatority(Handle_type::mandatory);
    console.add_cmd("device", std::move(handle4));

    console.add_cmd("baud_rate", Handle::Policies<Numbers_only>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = true;
                                       current_device->second.info.baud_rate = baud_rate_trans(str);
                                   }));

    console.add_cmd("parity", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = true;
                                       current_device->second.info.parity = parity_trans(str);
                                   }));

    console.add_cmd("char_size", Handle::Policies<Numbers_only>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = true;
                                       current_device->second.info.char_size = char_size_trans(str);
                                   }));

    console.add_cmd("flow_ctrl", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = true;
                                       current_device->second.info.flow_ctrl = flow_ctrl_trans(str);
                                   }));

    console.add_cmd("stop_bits", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = true;
                                       current_device->second.info.stop_bits = stop_bits_trans(str);
                                   }));

    console.add_cmd("settings_known", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.settings_known = settings_known_trans(str);
                                   }));

    console.add_cmd("settings_known", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       auto current_device = get_current();
                                       current_device->second.write_access = write_access_trans(str);
                                   }));

    /* App settings */
    console.add_cmd("debug", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       data_pack.debug = bool_trans(str);
                                   }));

    console.add_cmd("close_on_timeout", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       data_pack.close_on_timeout = bool_trans(str);
                                   }));

    console.add_cmd("auto_reload", Handle::Policies<>::Dyn_handle(
                                   [this](std::string &&str)
                                   {
                                       data_pack.auto_reload = bool_trans(str);
                                   }));
}
