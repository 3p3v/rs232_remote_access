#include <Traffic_receiver.hpp>
#include <algorithm>

namespace Mqtt_port
{
    void Traffic_receiver::pass(std::string channel_name, const Executor::Data_type &data)
    {
        channels[channel_name].get()->exec(data);
    }

    void Traffic_receiver::validate(std::string channel_name)
    {
        if (!channels[channel_name])
            throw std::logic_error("User isn't connected to specyfied channel: " + channel_name + ".");
    }
}