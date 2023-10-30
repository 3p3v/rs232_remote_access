#pragma once

#include <string>
#include <unordered_set>
#include <memory>
#include <Validator.hpp>

namespace Mqtt_port
{
    class Connector
    {
        using Cont_t = std::unordered_set<std::string>;

        Cont_t channels;
        std::shared_ptr<Validator> validator;

    public:
        Connector(std::shared_ptr<Validator> validator);

        /* Get next channel */
        std::string get_channel_to_con();

        /* Number of channels waiting to be connected to */
        size_t remaining_channels();
        
        /* Load channels that client needs to connect to, returns number of loaded channels */
        size_t load_channels();
    };
}