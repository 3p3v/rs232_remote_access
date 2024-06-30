#pragma once

#include <iostream>
#include <string>
/* User and server settings */
#include <User.hpp>
#include <Server.hpp>
/* Device record */
#include <Info_pack_base.hpp>
/* Cmd management */
#include <Setup_console.hpp>

using namespace Cmd_ctrl;
using namespace Logic;

namespace Impl
{
    /// @brief Config file reader and interpreter
    class Setup_impl
    {
        class Info_pack : public Info_pack_base
        {
        public:
            bool port_set{false};

            bool operator==(const Info_pack &ip);

            bool operator==(const std::string &ip);

            Info_pack(const std::string &&info); // TODO add both name and port - rewrite ctrl_console module
        };

    public:
        /// @brief Application options
        class App_opts
        {
        public:
            bool close_on_timeout{true};
            bool close_on_data_loss{true};
            bool close_on_protocol_error{true};
            /// @brief Show debug messages
            bool debug{false};
        };

    protected:
        using Device_cont = std::vector<Info_pack>;
        using Device_iter = Device_cont::iterator;

    public:
        /// @brief All nessesary information to run application and construct devices
        class Data_pack
        {
        public:
            App_opts app_opts{};
            Mqtt_port::Server::Cont server{};
            Mqtt_port::User::Cont user{};
            Device_cont devices;
        };

    protected:
        Setup_console console;
        using Command = Setup_console::Command;
        using Mandatoriness = Setup_console::Mandatoriness;

        /// @brief Data loaded from file
        Data_pack data_pack;

        /// @brief If data was loaded
        bool data_loaded{false};

        /// @brief Itr to the device that is being set up
        Device_iter current_{data_pack.devices.end()};

        /// @brief Get current device reference
        /// @return 
        Info_pack &get_current();

    public:
        Setup_impl();
    };
}