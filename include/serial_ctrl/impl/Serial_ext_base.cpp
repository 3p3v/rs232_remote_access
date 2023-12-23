#include <Serial_ext_base.hpp>

namespace Impl
{
    std::unique_ptr<Serial> &Serial_base::get_serial_ptr()
    {
        return serial->serial;
    }

    Serial_ext_base::Serial_ext_base(std::shared_ptr<Serial_helper> &&serial)
        : Serial_base{std::move(serial)}
    {
    }
}