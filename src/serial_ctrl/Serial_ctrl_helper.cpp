#include <Serial_ctrl_helper.hpp>

namespace Ip_serial
{
    Serial_ctrl_helper::Serial_ctrl_helper(std::shared_ptr<Serial_port::Serial> &&serial,
                                           Serial_info &&info,
                                           bool settings_known)
        : serial{std::move(serial)},
          info{std::move(info)},
          settings_known{settings_known}
    {
    }
}