#pragma once

#include <memory>
#include <stdexcept>
#include <impl/Controller.hpp>
#include <Device.hpp>
#include <Ip_console.hpp>
#include <Ip_serial_ctrl.hpp>

using namespace Mqtt_port;

namespace Ip_serial
{
    class Controller final : protected Callbacks_defs
    {
        static constexpr unsigned char qos{1};

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

        std::shared_ptr<Ip_serial_ctrl> add_serial(Device &device, Ip_serial_ctrl &&serial_ctrl)
    };

    Controller::Controller(Server::Get_cont &&server,
                           User::Get_cont &&user)
        : controller{std::move(server), std::move(user)}
    {
    }

    std::shared_ptr<Ip_serial_ctrl> Controller::add_serial(Device &device, Ip_serial_ctrl &&serial_ctrl_)
    {
        auto serial_ctrl = std::make_shared<Ip_serial_ctrl>(std::move(serial_ctrl_));
        
        monitor.validate(device.name());

        controller.subscribe(
            device_ptr->get_info_ch(),
            qos,
            [serial_ctrl = serial_ctrl_](const std::string &ch_name, const std::string::const_iterator begin, const std::string::const_iterator end)
            {
                /* Interpret received data */
                serial_ctrl->exec(begin, end); },
            [](int, std::string &) {});

        controller.subscribe(
            device_ptr->get_data_ch(),
            qos,
            [serial_ctrl = serial_ctrl_](const std::string &ch_name, const std::string::const_iterator begin, const std::string::const_iterator end)
            {
                /* Increment number of data that was received */
                /* Write data to phisical port */
                serial_ctrl->write(begin, end); },
            [](int, std::string &) {});
    }

}