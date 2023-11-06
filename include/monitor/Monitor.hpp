#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Device;

namespace Ip_serial
{
    class Ip_serial_ctrl;
}

namespace Phy_serial
{
    class Serial_ctrl;
}

/// @brief An interface for resource monitoring
class Monitor
{
protected:
    using Device_ptr = std::shared_ptr<const Device>;
    using Ip_serial_ctrl_ptr = std::shared_ptr<const Ip_serial::Ip_serial_ctrl>;
    using Serial_ctrl_ptr = std::shared_ptr<const Phy_serial::Serial_ctrl>;
    using Serial_pair = std::pair<Ip_serial_ctrl_ptr, Serial_ctrl_ptr>;

    std::unordered_map<Device_ptr, Serial_pair> devices;

public:
    virtual void wake(const Device_ptr &device) = 0;
    virtual void wake_delete(const Device_ptr &device) = 0;
    virtual void validate(const std::string &name) = 0;
    void add_device(Device_ptr device,
                    Ip_serial_ctrl_ptr ip_serial,
                    Serial_ctrl_ptr serial);

    Monitor();
    Monitor(Monitor&&) = default;
    Monitor& operator=(Monitor&&) = default;
    Monitor(const Monitor&) = default;
    Monitor& operator=(const Monitor&) = default;
    virtual ~Monitor() = default;
};
