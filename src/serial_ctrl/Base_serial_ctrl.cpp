#pragma once

#include <Base_serial_ctrl.hpp>
// #include <Ip_console.hpp>

const Flow Base_serial_ctrl::get_flow() const
{
    return flow;
}

Base_serial_ctrl::Base_serial_ctrl(std::shared_ptr<const Device> device)
    : device{std::move(device)}
{
}
