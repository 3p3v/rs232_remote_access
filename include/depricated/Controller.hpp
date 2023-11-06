#pragma once

#include <memory>
#include <stdexcept>
#include <impl/Controller.hpp>
#include <Device.hpp>
#include <Ip_console.hpp>
#include <Ip_serial_ctrl.hpp>
#include <Mqtt_defs.hpp>
#include <Serial_ctrl.hpp>

using namespace Mqtt_port;

namespace Ip_serial
{
    class Controller final : protected Mqtt_defs
    {
        using Ip_serial_ctrl_ptr = std::shared_ptr<Ip_serial_ctrl>;
        
        /* Low level implementation of mqtt client */
        Impl::Controller controller;
        Ip_console console;

    public:
        Controller(Server::Get_cont &&server,
                   User::Get_cont &&user);
        Controller(Controller &&) = default;
        Controller &operator=(Controller &&) = default;
        Controller(const Controller &) = delete;
        Controller &operator=(const Controller &) = delete;
        ~Controller() = default;

        void run();

        void disconnect(Time time);

        Ip_serial_ctrl_ptr Controller::add_serial(Base_serial_ctrl &&base_serial_ctrl, Serial_ctrl_helper &&helper);

        std::shared_ptr<Phy_serial::Serial_ctrl> bind_to_controller(Phy_serial::Ext_serial_ctrl &&serial_ctrl);
    };

    

    

    Controller::Ip_serial_ctrl_ptr Controller::add_serial(Base_serial_ctrl &&base_serial_ctrl, Serial_ctrl_helper &&helper)
    {
        auto serial_ctrl = std::make_shared<Ip_serial_ctrl>(std::move(base_serial_ctrl), std::move(helper), console);

        controller.subscribe(
            device.get_info_ch(),
            qos,
            [serial_ctrl](const std::string &ch_name, const std::string::const_iterator begin, const std::string::const_iterator end)
            {
                /* Interpret received data */
                serial_ctrl->exec(begin, end); },
            [](int, std::string &) {});

        controller.subscribe(
            device.get_data_ch(),
            qos,
            [serial_ctrl](const std::string &ch_name, const std::string::const_iterator begin, const std::string::const_iterator end)
            {
                /* Increment number of data that was received */
                /* Write data to phisical port */
                serial_ctrl->write(begin, end); },
            [](int, std::string &) {});
    }

}