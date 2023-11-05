#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <Device.hpp>
#include <Flow.hpp>
#include <Controller.hpp>
#include <Monitor.hpp>
// #include <Ip_console.hpp>

namespace Ip_serial
{
    class Console;

    class Base_serial_ctrl
    {
        const std::shared_ptr<const Device> name;
        const std::string channel_name;
        // const std::string com1;
        // const std::string com2;
        Flow data_out{0};
        Monitor& monitor;
        bool write_access;

    public:
        const Flow get_out() const;

        Base_serial_ctrl(std::shared_ptr<const Device> name,
                    Monitor& monitor, 
                    bool write_access);
        Base_serial_ctrl(Base_serial_ctrl &&) = default;
        Base_serial_ctrl &operator=(Base_serial_ctrl &&) = default;
        Base_serial_ctrl(const Base_serial_ctrl &) = delete;
        Base_serial_ctrl &operator=(const Base_serial_ctrl &) = delete;
        virtual ~Base_serial_ctrl() = 0;
    };
}