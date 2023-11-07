#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <Monitor.hpp>
// #include <ip_serial/Controller.hpp>
#include <impl/Controller.hpp>
#include <serial/Controller.hpp>
#include <Ip_defs.hpp>
#include <Serial_info.hpp>
#include <Serial_settings_intf.hpp>
#include <ip_serial/Ip_console.hpp>

namespace Main_serial
{
    /// @brief Used to create and manage connection
    class Controller final : public Serial_port::Ctrl_defs, public Serial_settings_intf
    {
        Phy_serial::Controller com_controller;
        Monitor &monitor;
        Mqtt_port::Impl::Controller ip_controller;
        Ip_serial::Ip_console cmds_console;

    public:
        Controller(Mqtt_port::Server::Get_cont &&server,
                   Mqtt_port::User::Get_cont &&user,
                   Monitor &monitor);

        /// @brief Run underlying ...//TODO
        void run();

        /// @brief Add device with unknown settings
        /// @param device 
        /// @param write_access 
        void add_device(Device &&device, bool write_access);

        /// @brief Add device with known settings
        /// @param device 
        /// @param info 
        /// @param write_access 
        void add_device(Device &&device, Serial_info&& info, bool write_access);

        /// @brief Add device
        /// @param device 
        /// @param info 
        /// @param write_access 
        /// @param settings_known 
        void add_device(Device &&device, Serial_info&& info, bool write_access, bool settings_known);

        void set_baud_rate(const std::string &dev_name, const unsigned int baud_rate) override;
        void set_parity(const std::string &dev_name, const Parity parity) override;
        void set_char_size(const std::string &dev_name, const unsigned int char_size) override;
        void set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl) override;
        void set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits) override;
        void disconnect_device(const std::string &dev_name);
        void close();
    };
}
