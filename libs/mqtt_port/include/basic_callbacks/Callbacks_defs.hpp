#pragma once

#include <string>

namespace Mqtt_port
{
    class Callbacks_defs
    {
    public:
        static constexpr unsigned int time_to_disconnect = 200;
        static constexpr unsigned char qos = 1;

    public:
        Callbacks_defs() = default;
        Callbacks_defs(Callbacks_defs&&) = default;
        Callbacks_defs& operator=(Callbacks_defs&&) = default;
        Callbacks_defs(Callbacks_defs&) = default;
        Callbacks_defs& operator=(Callbacks_defs&) = default;
        virtual ~Callbacks_defs() = default;
    };
}