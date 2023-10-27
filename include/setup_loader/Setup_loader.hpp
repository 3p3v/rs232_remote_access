#pragma once

#include <iostream>
#include <string>
#include <User.hpp>
#include <Server.hpp>
#include <Ctrl_console.hpp>
#include <Device.hpp>

using namespace Cmd_ctrl;

class Setup_loader final
{
public:
    class Data_pack
    {
    public:
        Mqtt_port::User user{};
        Mqtt_port::Server server{};
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
    console.add_cmd("username", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::optional,
                                                               [this](std::string &&str)
                                                               {
                                                                   data_pack.user.set_username(std::move(str));
                                                               }));

    console.add_cmd("password", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::optional,
                                                               [this](std::string &&str)
                                                               {
                                                                   data_pack.user.set_password(std::move(str));
                                                               }));

    console.add_cmd("user_id", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::optional,
                                                              [this](std::string &&str)
                                                              {
                                                                  data_pack.user.set_id(std::move(str));
                                                              }));

    console.add_cmd("ip", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::mandatory,
                                                         [this](std::string &&str)
                                                         {
                                                             data_pack.server.ip = std::move(str);
                                                         }));

    console.add_cmd("port", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::mandatory,
                                                           [this](std::string &&str)
                                                           {
                                                               data_pack.server.port = std::move(str);
                                                           }));

    console.add_cmd("pem", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::mandatory,
                                                          [this](std::string &&str)
                                                          {
                                                              data_pack.server.pem_file_name = std::move(str);
                                                          }));

    console.add_cmd("crt", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::mandatory,
                                                          [this](std::string &&str)
                                                          {
                                                              data_pack.server.crt_file_name = std::move(str);
                                                          }));

    console.add_cmd("device", Policies<>::make_def_dyn_proxy(Base_handle_proxy::Type::mandatory,
                                                             [this](std::string &&str)
                                                             {
                                                                 data_pack.device.set_name(std::move(str));
                                                             }));
}
