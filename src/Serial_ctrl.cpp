#pragma once

#include <Base_serial_ctrl.hpp>
// #include <Ip_console.hpp>

namespace Ip_serial
{
    std::pair<const std::string &, const std::string &> Serial_ctrl::get_coms()
    {
        return std::make_pair(com1, com2);
    }

    Serial_ctrl::Serial_ctrl(std::shared_ptr<const Device> name,
                             Mqtt_port::Impl::Controller &controller,
                             Monitor &monitor,
                             bool write_access,
                             Mqtt_port::Impl::Controller &controller,
                             std::string &&com1,
                             std::string &&com2)
        : Base_serial_ctrl{std::move(name), monitor{monitor}, write_access{write_access}},
          controller{controller},
          com1{std::move(com1)},
          com2{std::move(com2)}
    {
    }

    Serial_ctrl::Serial_ctrl(Base_serial_ctrl &&base,
                             Mqtt_port::Impl::Controller &controller
                             std::string &&com1,
                             std::string &&com2)
        : Base_serial_ctrl{std::move(base)},
          controller{controller},
          com1{std::move(com1)},
          com2{std::move(com2)}
    {
    }

}