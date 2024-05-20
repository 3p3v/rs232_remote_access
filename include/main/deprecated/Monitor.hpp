#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <Exception.hpp>
#include <Main_defs.hpp>
#include <Setup_loader.hpp>

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
        Setup_loader::App_opts app_opts{};
        Controller &controller;

    public:
        void error(const Exception::Exception &except);
        template <typename Str>
        void debug(const Device_ptr &device, Str &&info);
        void wake(const Device_ptr &device);
        void wake_delete(const Device_ptr &device);
        void run();
        void set_opts(Setup_loader::App_opts &&app_opts);

        Monitor(Controller &controller, const std::unordered_map<Device_ptr, Serial_pair> &devices);
        Monitor(Monitor &&) = delete;
        Monitor &operator=(Monitor &&) = delete;
        Monitor(const Monitor &) = delete;
        Monitor &operator=(const Monitor &) = delete;
        ~Monitor() = default;
    };

    template <typename Str>
    void Monitor::debug(const Device_ptr &device, Str &&info)
    {
        if (app_opts.debug)
        {
            std::cout << "DEBUG: " << device->get_data_ch() << " MSG: " << info << '\n';
        }
    }
}
