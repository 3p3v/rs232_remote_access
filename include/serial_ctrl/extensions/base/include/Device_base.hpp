#pragma once

namespace Logic
{
    class Device_base
    {
    public:
        Device_base() = default;
        Device_base(Device_base&&) = default;
        Device_base& operator=(Device_base&&) = default;
        Device_base(const Device_base&) = default;
        Device_base& operator=(const Device_base&) = default;
        virtual ~Device_base() = 0;
    };
} // namespace Logic
