#include <Serial_ctrl_helper.hpp>

namespace Ip_serial
{
    Serial_ctrl_helper::Serial_ctrl_helper(std::shared_ptr<Serial_port::Serial> &&serial,
                                           Serial_info &&info,
                                           Monitor &monitor,
                                           bool settings_known)
        : serial{std::move(serial)},
          info{std::move(info)},
          timers{monitor},
          settings_known{settings_known}
    {
    }
}