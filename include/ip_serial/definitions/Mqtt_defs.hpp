#pragma once

#include <string>

namespace Ip_serial
{
    class Mqtt_defs
    {
    public:
        static constexpr unsigned int time_to_disconnect = 200;
        static constexpr unsigned char qos = 2;

    public:
        Mqtt_defs() = default;
        Mqtt_defs(Mqtt_defs&&) = default;
        Mqtt_defs& operator=(Mqtt_defs&&) = default;
        Mqtt_defs(Mqtt_defs&) = default;
        Mqtt_defs& operator=(Mqtt_defs&) = default;
        virtual ~Mqtt_defs() = 0;
    };
}