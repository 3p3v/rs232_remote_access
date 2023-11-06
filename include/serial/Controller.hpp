#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <Dyn_serial.hpp>
#include <Device.hpp>
#include <Serial_ctrl.hpp>

namespace Phy_serial
{
    class Controller final
    {
    public:
        using Com_pair = std::pair<std::string, std::string>;
        using Serial_ptr = std::unique_ptr<Serial_port::Serial>;
        using Serial_ctrl_ptr = std::shared_ptr<Phy_serial::Serial_ctrl>;

        Controller() = default;
        Controller(Controller &&) = default;
        Controller &operator=(Controller &&) = default;
        Controller(const Controller &) = delete;
        Controller &operator=(const Controller &) = delete;
        ~Controller() = default;

        Com_pair create_virtual_coms();
        // template <typename Str>
        // Serial_ptr add_serial(Str &&com, Serial_ctrl_ptr &&serial_ctrl);
        // void close_serial(std::shared_ptr<Serial_get> &&serial_ctrl);
    };

    // template <typename Str>
    // Controller::Serial_ptr Controller::add_serial(Str &&com, Serial_ctrl_ptr &&serial_ctrl)
    // {
        
    // }
}