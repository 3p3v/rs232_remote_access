#pragma once

#include <memory>
#include <Serial_info.hpp>
#include <Serial.hpp>

namespace Ip_serial
{
    class Serial_ext : public Serial_info
    {
    public:
        

        Serial_ext(std::unique_ptr<Serial_port::Serial> &&serial);
    };
}
