#pragma once

#include <string>
#include <unordered_set>
#include <memory>

namespace Mqtt_port
{
    class Connector
    {
        using Cont_t = std::unordered_set<std::string>;

        Cont_t channels;

    public:
        /* Get next channel */
        std::string get_channel_to_con();

        /* Number of channels waiting to be connected to */
        size_t remaining_channels();
        
        /* Load channels that client needs to connect to, returns number of loaded channels */
        size_t load_channel(std::string &&ch_name);
    };
}