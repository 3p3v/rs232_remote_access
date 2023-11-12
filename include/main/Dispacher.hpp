#pragma once

#include <memory>
#include <Main_defs.hpp>
#include <Monitor.hpp>

using namespace Main_serial;

class Dispacher
{
    static std::unordered_map<Device_ptr, Serial_pair> devices;
    Dispacher() = default;

public:
    static std::unordered_map<Device_ptr, Serial_pair> &get_devices()
    {
        return devices;
    }
};
