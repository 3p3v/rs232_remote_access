#pragma once

#include <memory>

namespace Ip_serial
{
    class Ip_serial_ctrl;
}

namespace Phy_serial
{
    class Serial_ctrl;
}

class Device;

namespace Main_serial
{
    using Device_ptr = std::shared_ptr<const Device>;
    using Ip_serial_ctrl_ptr = std::shared_ptr<const Ip_serial::Ip_serial_ctrl>;
    using Serial_ctrl_ptr = std::shared_ptr<const Phy_serial::Serial_ctrl>;
    using Serial_pair = std::pair<Ip_serial_ctrl_ptr, Serial_ctrl_ptr>;
}