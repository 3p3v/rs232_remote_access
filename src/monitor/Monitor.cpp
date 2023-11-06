#include <Monitor.hpp>

void Monitor::add_device(Device_ptr device,
                    Ip_serial_ctrl_ptr ip_serial,
                    Serial_ctrl_ptr serial)
{
    devices.emplace(std::move(device), std::make_pair(std::move(ip_serial), std::move(serial)));
}