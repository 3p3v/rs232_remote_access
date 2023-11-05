#pragma once

#include <Base_serial_ctrl.hpp>
// #include <Ip_console.hpp>

namespace Ip_serial
{
    const Flow Serial_get::get_in()
    {
        return data_in;
    }
    
    Base_serial_ctrl::Base_serial_ctrl(std::shared_ptr<const Device> name,
                             Monitor &monitor,
                             bool write_access)
        : name{std::move(name), monitor{monitor}, write_access{write_access}, channel_name{name->get_data_ch()}}
    {
    }

    Base_serial_ctrl::~Base_serial_ctrl() = default;
}