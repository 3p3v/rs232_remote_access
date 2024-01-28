#pragma once

#include <Serial_base.hpp>

namespace Impl
{
    class Serial_ext_base : public Serial_base
    {
    public:
        std::unique_ptr<Serial> &get_serial_ptr();

        Serial_ext_base(std::shared_ptr<Serial_helper> &&serial);
    }
}