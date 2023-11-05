#include <Connector.hpp>

namespace Mqtt_port
{
    // Connector::Connector(std::shared_ptr<Validator> validator)
    //     : validator{std::move(validator)}
    // {
    // }

    
    size_t Connector::load_channel(std::string &&ch_name)
    {
        channels.insert(std::move(ch_name));
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

    
    // size_t Connector::connect_to_next()
    // {
    //     if (remaining_channels())
    //     {
    //         conn_handle(get_channel_to_con());
    //     }

    //     return remaining_channels();
    // }

    // Paho_connector::Paho_connector(std::shared_ptr<mqtt::async_client> client,
    //                                std::shared_ptr<Validator> &&validator,
    //                                std::unique_ptr<mqtt::iaction_listener> &&callb)
    //     : Connector{std::move(validator)}, client{client}, callb{std::move(callb)}
    // {

    // }

    // void Paho_connector::conn_handle(const std::string channel_name)
    // {
    //     client.get()->connect(options, nullptr, *callb);
    // }
}