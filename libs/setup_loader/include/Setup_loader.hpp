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
/* Policies */
#include <Numbers_only.hpp>

using namespace Cmd_ctrl;
using namespace Logic;

namespace Impl
{
    class Setup_loader final
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
        class App_opts
        {
        public:
            bool close_on_timeout{true};
            bool close_on_data_loss{true};
            bool close_on_protocol_error{true};
            bool debug{false};
        };

    private:
        using Device_cont = std::vector<Info_pack>;
        using Device_iter = Device_cont::iterator;

    public:
        class Data_pack
        {
        public:
            App_opts app_opts{};
            Mqtt_port::Server::Cont server{};
            Mqtt_port::User::Cont user{};
            Device_cont devices;
        };

    private:
        Setup_console console;
        using Command = Setup_console::Command;
        using Mandatority = Setup_console::Mandatority;

        /// @brief Data loaded from file
        Data_pack data_pack;

        /// @brief If data was loaded
        bool data_loaded{false};

        /// @brief Itr to the device that is being set up
        Device_iter current_{data_pack.devices.end()};

        /// @brief Get current device reference
        /// @return 
        Info_pack &get_current();

        void load_data_handler(const std::string &file_name);

    public:
        Setup_loader();

        Data_pack &load_data(const std::string &file_name) &;
        Data_pack load_data(const std::string &file_name) &&;

        Data_pack &get_data_pack() &;
    };
}