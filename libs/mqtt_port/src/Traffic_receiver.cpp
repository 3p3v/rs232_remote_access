#include <Traffic_receiver.hpp>
#include <algorithm>

namespace Mqtt_port
{
    void Traffic_receiver::read(const std::string &channel_name, const Executor::Data &data)
    {
        channels[channel_name].get()->exec(data);
    }

    void Traffic_receiver::validate(std::string channel_name)
    {
        if (!channels[channel_name])
            throw std::logic_error("User isn't connected to specyfied channel: " + channel_name + ".");
    }

    std::unordered_set<std::string> Traffic_receiver::get_channels()
    {
        std::unordered_set<std::string> channels_cpy(channels.size());
        std::for_each(channels.begin(),
                      channels.end(),
                      [&channels_cpy](const auto& ch)
                      {
                        channels_cpy.insert(ch.first);
                      });

        return channels_cpy;
    }
}