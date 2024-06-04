#pragma once

#include <string>

namespace Logic
{
    class Mqtt_defs
    {
    public:
        static constexpr unsigned int time_to_disconnect = 200;
        static constexpr unsigned char qos = 2;

    public:
        Mqtt_defs() = delete;
    };
}