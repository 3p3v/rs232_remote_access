#include <Serial_ext.hpp>

namespace Ip_serial
{
    Serial_ext::Serial_ext(std::unique_ptr<Serial_port::Serial> &&serial)
        : serial{std::move(serial)}
    {
    }
}