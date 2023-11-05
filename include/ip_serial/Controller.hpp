#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <Monitor.hpp>
#include <ip_serial/controller/Controller.hpp>
#include <serial/controller/Controller.hpp>

using namespace Cmd_ctrl::Ctrl;

namespace Main_controller
{
    class Controller final : protected Ip_defs, public Serial_port::Ctrl_defs
    {
        using Handle = Ctrl::Handle_t;

        Ip_serial::Controller ip_controller;
        Serial::Controller controller;
        Monitor monitor;
        // Ip_console console;
        // std::map<std::string, std::shared_ptr<Serial_port::Serial>> serials;
        // std::shared_ptr<Flow_monitor> flow_monitor{new Flow_monitor{}};

    public:
        Controller(Mqtt_port::Server::Get_cont &&server,
                   Mqtt_port::User::Get_cont &&user);

        void run();

        // template <typename Device_t>
        void add_device(Device &&device);
        // Serial_port::Serial &operator[](const std::string &dev_name);

        void set_baud_rate(const std::string &dev_name, const unsigned int baud_rate);
        void set_parity(const std::string &dev_name, const Parity parity);
        void set_char_size(const std::string &dev_name, const unsigned int char_size);
        void set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl);
        void set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits);
        void close(const std::string &dev_name);
    };

    // template <typename Device_t> //, std::enable_if_t<std::is_same_v<Device_t, Device>>*>
    void Controller::add_device(Device &&device, Serial_info &&info, bool write_access)
    {
        auto device_ptr = std::make_shared<Device>(std::forward<Device_t>(device));

        /* Create and bind ports */

        auto serial_pack = controller.add_serial(Base_serial_ctrl{device_ptr,
                                                                  monitor,
                                                                  write_access});

        auto = ip_controller.add_serial
    }
}
