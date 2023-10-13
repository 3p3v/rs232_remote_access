#include <Connector.hpp>

namespace Mqtt_port
{
    Connector::Connector(std::shared_ptr<Validator> validator)
        : validator{validator}
    {
    }

    
    size_t Connector::load_channels()
    {
        channels = std::move(validator.get()->get_channels());
        return channels.size(); 
    }

    
    size_t Connector::remaining_channels()
    {
        return channels.size();
    }

    
    std::string Connector::get_channel_to_con()
    {
        std::string channel{*channels.begin()};
        channels.erase(channels.begin());
        return channel;
    }

    
    size_t Connector::connect_to_next()
    {
        if (remaining_channels())
        {
            conn_handle(get_channel_to_con());
        }

        return remaining_channels();
    }
}