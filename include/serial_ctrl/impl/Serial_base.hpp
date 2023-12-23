#pragma once

#include <Serial_helper.hpp>

namespace Impl
{
    class Serial_base
    {
        using Serial = Serial_helper::Serial;

    protected:
        std::shared_ptr<Serial_helper> serial;

    public:
        Serial &get_serial();

        Serial_base(std::shared_ptr<Serial_helper> &&serial);
        Serial_base(const Serial_base &) = delete;
        Serial_base(Serial_base &&) = default;
        Serial_base &operator=(const Serial_base &) = delete;
        Serial_base &operator=(Serial_base &&) = default;
        virtual ~Serial_base() = default;
    };
}
