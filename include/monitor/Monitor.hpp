#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <Errors.hpp>

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
    virtual void error(Errors type) = 0;
    virtual void error(Errors type, std::string&& what) = 0;
    virtual void error(Errors type, const std::string& what) = 0;
    virtual void error(Errors type, int code) = 0;
    virtual void error(Errors type, int code, std::string&& what) = 0;
    virtual void error(Errors type, int code, const std::string& what) = 0;
    virtual void wake(const Device_ptr &device) = 0;
    virtual void wake_delete(const Device_ptr &device) = 0;
    virtual void validate(const std::string &name) = 0;
    void add_device(Device_ptr device,
                    Ip_serial_ctrl_ptr ip_serial,
                    Serial_ctrl_ptr serial);

    Monitor() = default;
    Monitor(Monitor&&) = default;
    Monitor& operator=(Monitor&&) = default;
    Monitor(const Monitor&) = default;
    Monitor& operator=(const Monitor&) = default;
    virtual ~Monitor() = default;
};
