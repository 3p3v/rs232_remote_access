#include <Traffic_channel_connector.hpp>

namespace Mqtt_port
{
    unsigned int Traffic_channel_connector::load_channels(Cont_type &&cont)
    {
        channels = cont;
    }

    unsigned int Traffic_channel_connector::remaining_channels()
    {
        return channels.size();
    }

    std::string Traffic_channel_connector::get_channel_to_con()
    {
        std::string channel = std::move(channels.back());
        channels.pop_back();
        return channel;
    }
}