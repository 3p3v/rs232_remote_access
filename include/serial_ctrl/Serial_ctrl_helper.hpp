#pragma once

#include <memory>
#include <Serial_info.hpp>
#include <Serial.hpp>

namespace Ip_serial
{
    class Serial_ctrl_helper
    {
    public:
        std::unique_ptr<Serial_port::Serial> serial; // 2
        Serial_info info;                                  // 2
        bool settings_known;                               // 2

        Serial_ctrl_helper(std::unique_ptr<Serial_port::Serial> &&serial,
                           Serial_info &&info,
                           bool settings_known);
        Serial_ctrl_helper(Serial_ctrl_helper&&) = default;
        Serial_ctrl_helper& operator=(Serial_ctrl_helper&&) = default;
        Serial_ctrl_helper(const Serial_ctrl_helper&) = default;
        Serial_ctrl_helper& operator=(const Serial_ctrl_helper&) = default;
        ~Serial_ctrl_helper() = default;
    };
}