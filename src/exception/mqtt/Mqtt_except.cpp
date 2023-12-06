#include <Mqtt_except.hpp>

namespace Exception
{
    Mqtt_except::Mqtt_except(const mqtt::exception& except)
        : Exception{"MQTT", except.what()}
    {
    }

    Mqtt_except::Mqtt_except(const std::string& expln)
        : Exception{"MQTT", expln}
    {
    }

    Mqtt_except::Mqtt_except(const char *expln)
        : Exception{"MQTT", expln}
    {
    }

    Mqtt_write_except::Mqtt_write_except()
        : Mqtt_except{"Error while publishing.\nPossible write access restriction."}
    {
    }

    Mqtt_read_except::Mqtt_read_except()
        : Mqtt_except{"Error while subscribing.\nPossible read access restriction."}
    {
    }
}
