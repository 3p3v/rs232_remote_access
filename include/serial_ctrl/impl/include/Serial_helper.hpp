#pragma once

#include <memory>

namespace Serial_port
{
    class Serial;
}

namespace Impl
{
    class Serial_helper
    {
    public:
        using Serial = Serial_port::Serial;

        std::unique_ptr<Serial> serial{nullptr};
    };
}