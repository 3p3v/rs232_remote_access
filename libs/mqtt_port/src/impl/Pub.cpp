#include <Pub.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        Pub::Pub(std::string &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data, std::unique_ptr<mqtt::iaction_listener> &&callb)
            : Sub{std::move(channel_name), qos, std::move(callb)}, data(std::move(data))
        {

        }

        Pub::Pub(std::string &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data)
            : Sub{std::move(channel_name), qos}, data(std::move(data))
        {
        }
    }
}