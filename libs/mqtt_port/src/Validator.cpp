#include <Validator.hpp>
#include <algorithm>

namespace Mqtt_port
{
    Validator::Validator()
        : channels() 
    {
    }

    Validator::Executor_ptr Validator::get_exec(const std::string &channel_name)
    {
        return channels[channel_name];
    }

    bool Validator::validate(std::string channel_name)
    {
        if (!channels[channel_name])
            return false;
        else 
            return true;
    }

    std::unordered_set<std::string> Validator::get_channels()
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