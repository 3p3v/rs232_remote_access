#pragma once

#include <Exception.hpp>
#include <mqtt/exception.h>

namespace Exception
{
    /// @brief MQTT unknown exception
    class Mqtt_except : public Exception
    {
    public:
        Mqtt_except(const mqtt::exception& except);
        Mqtt_except(const std::string& expln);
        Mqtt_except(const char *expln);
    };

    class Mqtt_write_except : public Mqtt_except
    {
    public:
        Mqtt_write_except();
    }; 

    class Mqtt_read_except : public Mqtt_except
    {
    public:
        Mqtt_read_except();
    };    
}
