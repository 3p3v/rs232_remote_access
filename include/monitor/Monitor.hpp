#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <Exception.hpp>
#include <Main_defs.hpp>

class Device;

namespace Ip_serial
{
    class Ip_serial_ctrl;
}

namespace Phy_serial
{
    class Serial_ctrl;
}

namespace Main_serial
{
    class Controller;
}

namespace Main_serial
{
    /// @brief An interface for resource monitoring
    class Monitor
    {
        const std::unordered_map<Device_ptr, Serial_pair> &devices;
        Controller &controller;

    public:
        void error(const Exception::Exception &except);
        void wake(const Device_ptr &device);
        void wake_delete(const Device_ptr &device);
        void run();

        Monitor(Controller &controller, const std::unordered_map<Device_ptr, Serial_pair> &devices);
        Monitor(Monitor &&) = delete;
        Monitor &operator=(Monitor &&) = delete;
        Monitor(const Monitor &) = delete;
        Monitor &operator=(const Monitor &) = delete;
        ~Monitor() = default;
    };
}
