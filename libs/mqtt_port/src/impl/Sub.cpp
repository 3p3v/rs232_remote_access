#include <Sub.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        bool Sub::util_callb()
        {
            if (callb)
                return true;
            else
                return false;
        }

        Sub::Sub(std::string &&channel_name, unsigned char qos, std::unique_ptr<mqtt::iaction_listener> &&callb)
            : channel_name{std::move(channel_name)}, qos{qos}, callb{std::move(callb)}
        {

        }
        Sub::Sub(std::string &&channel_name, unsigned char qos)
            : channel_name{std::move(channel_name)}, qos{qos}
        {
        }
    }
}
