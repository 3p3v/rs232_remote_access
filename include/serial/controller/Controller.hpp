#pragma once

#include <memory>
#include <stdexcept>
#include <Dyn_serial.hpp>
#include <Device.hpp>
#include <Serial_ctrl.hpp>

using namespace Mqtt_port;

namespace Serial
{
    class Controller final
    {
    public:
        using Serial_pair = std::pair<std::unique_ptr<Serial_port::Serial>, std::shared_ptr<Serial_ctrl>>;

        Controller() = default;
        Controller(Controller &&) = default;
        Controller &operator=(Controller &&) = default;
        Controller(const Controller &) = delete;
        Controller &operator=(const Controller &) = delete;
        ~Controller() = default;

        Serial_pair add_serial(Device &device, Base_serial_ctrl &&serial_ctrl);
        void close_serial(std::shared_ptr<Serial_get> &&serial_ctrl);
    };

    Serial_pair Controller::add_serial(Base_serial_ctrl &&serial_ctrl)
    {
        // TODO create virtual serial
        
        /* Add phisical port */
        auto serial_ctrl_ = std::make_shared<Serial_ctrl>(std::move(serial_ctrl), , "CNCA0", "CNCB0");

        auto serial = std::make_unique<Dyn_serial>("CNCA0",
                                             [](std::size_t write_len)
                                             {
                                                /* Not used */ 
                                             },
                                             [serial_ctrl = serial_ctrl_](const Serial_port::Data_interface::Data &data, std::size_t read_len)
                                             {
                                                /* Increment number of data that has to be sent */
                                                /* Write data to channel */
                                                serial_ctrl->write(data.cbegin(), data.cbegin() + read_len);
                                             },
                                             [](const unsigned int code, const std::string &err)
                                             {
                                                /* Send error to error handling object */
                                             });
        serial->run();
        return std::amke_pair(std::move(serial), std::move(serial_ctrl_));
    }

    void Controller::close_serial(std::shared_ptr<Serial_get> &&serial_ctrl)
    {
        // TODO delete created virtual port
    }

}