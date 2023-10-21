#pragma once

namespace Mqtt_port
{
    class Defs
    {
    protected:
        static constexpr unsigned int time_to_disconnect = 200;
        static constexpr unsigned char qos = 1;
    };
}