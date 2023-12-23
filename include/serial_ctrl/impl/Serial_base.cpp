#include <Serial_base.hpp>

namespace Impl
{
    Serial &Serial_settings::get_serial()
    {
        if (serial->serial->get() != nullptr)
        {
            return *serial->serial->get();
        }
        else
        {
            throw std::logic_error{"Serial not initialized, action can't be executed!"};
        }
    }

    Serial_base::Serial_base(std::shared_ptr<Serial_helper> &&serial)
        : serial{std::move(serial)}
    {
    }
}