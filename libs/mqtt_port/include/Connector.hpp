#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include <Validator.hpp>

namespace Mqtt_port
{
    
    class Connector
    {
        using Cont_type = std::unordered_set<std::string>;

        Cont_type channels;
        std::shared_ptr<Validator> validator;

        size_t remaining_channels();
        /* Returns number of channels left to connect */
        std::string get_channel_to_con();

    protected:
        virtual void conn_handle(const std::string channel_name) = 0;

    public:
        Connector(std::shared_ptr<Validator> validator);

        size_t connect_to_next();

        /* Load channels that need to be connected, returns number of loaded channels */
        size_t load_channels();
    };

    
    
}