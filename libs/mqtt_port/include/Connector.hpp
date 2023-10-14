#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include <Validator.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    
    class Connector// : public Validator
    {
        using Cont_t = std::unordered_set<std::string>;

        Cont_t channels;
        std::shared_ptr<Validator> validator;

        /* Returns number of channels left to connect */
        
        

    protected:
        // size_t connect_to_next();
        
        
        // virtual void conn_handle(const std::string channel_name) = 0;

    public:
        Connector(std::shared_ptr<Validator> validator);
        std::string get_channel_to_con();
        size_t remaining_channels();
        

        /* Load channels that need to be connected, returns number of loaded channels */
        size_t load_channels();
    };



    // class Paho_connector final : public Connector
    // {
    //     std::shared_ptr<mqtt::async_client> client;
    //     std::unique_ptr<mqtt::iaction_listener> callb;
    //     mqtt::connect_options options;

    // protected:
    //     void conn_handle(const std::string channel_name);

    // public:
    //     Paho_connector(std::shared_ptr<mqtt::async_client> client,
    //                    std::shared_ptr<Validator> &&validator,
    //                    std::unique_ptr<mqtt::iaction_listener> &&callb);
    // };

    
    
}