#include <Serial_ctrl.hpp>

namespace Ip_serial
{
    Ip_serial_ctrl::Ip_serial_ctrl(std::shared_ptr<const Device> name,
                                   std::unique_ptr<Serial_port::Serial> &&serial,
                                   Ip_serial::Console &console,
                                   Serial_info &&info,
                                   Monitor &monitor,
                                   bool settings_known)
        : name{std::move(name)},
          serial{std::move(name)},
          console{console},
          info{std::move(info)},
          monitor{monitor},
          settings_known{settings_known}
    {
    }

    Ip_serial_ctrl::~Ip_serial_ctrl()
    {
      monitor.wake_delete(name);
    };

    const Serial_info &Ip_serial_get::get_info()
    {
        return serial_info;
    }

    const Flow Ip_serial_get::get_in()
    {
        return data_in;
    }
}