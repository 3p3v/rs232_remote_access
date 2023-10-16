#pragma once

#include <Def.hpp>

namespace Mqtt_port
{
    class Disconnector_intf
    {
    public:
        virtual void disconnect(Time time) = 0;
        // virtual void subscribe();
    };
}