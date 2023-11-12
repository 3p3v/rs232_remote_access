#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <Device.hpp>
#include <Flow.hpp>
// #include <Controller.hpp>
#include <Monitor.hpp>
// #include <Ip_console.hpp>

class Console;

class Base_serial_ctrl
{
protected:
    const std::shared_ptr<const Device> device; // 0
    Flow flow;                                  // 0

public:
    const Flow get_flow() const;

    Base_serial_ctrl(std::shared_ptr<const Device> device);
    Base_serial_ctrl(Base_serial_ctrl &&) = default;
    Base_serial_ctrl &operator=(Base_serial_ctrl &&) = default;
    Base_serial_ctrl(const Base_serial_ctrl &) = default;
    Base_serial_ctrl &operator=(const Base_serial_ctrl &) = default;
    virtual ~Base_serial_ctrl() = default;
};
