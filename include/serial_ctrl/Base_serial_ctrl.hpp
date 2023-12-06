#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <Device.hpp>
#include <Flow.hpp>
// #include <Controller.hpp>
#include <Monitor.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Ip_master.hpp>

class Console;

class Base_serial_ctrl
{
protected:
    const std::shared_ptr<const Device> device; 
    /* Messages saving */
    std::shared_ptr<Ip_serial::Mqtt_msg_cont> msgs{std::make_shared<Ip_serial::Mqtt_msg_cont>()};
    Flow flow;   
    std::shared_ptr<Ip_serial::Ip_master> master_counter{std::make_shared<Ip_serial::Ip_master>()};                               

public:
    const Flow get_flow() const;

    Base_serial_ctrl(std::shared_ptr<const Device> device);
    Base_serial_ctrl(Base_serial_ctrl &&) = default;
    Base_serial_ctrl &operator=(Base_serial_ctrl &&) = default;
    Base_serial_ctrl(const Base_serial_ctrl &) = default;
    Base_serial_ctrl &operator=(const Base_serial_ctrl &) = default;
    virtual ~Base_serial_ctrl() = default;
};
