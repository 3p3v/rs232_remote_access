#pragma once

#include <string>
#include <unordered_set>

namespace Mqtt_port
{
    class Traffic_channel_connector
    {
        using Cont_type = std::unordered_set<std::string>;

        Cont_type channels;

    protected:
        /* Load channels that need to be connected, returns number of loaded channels */
        unsigned int load_channels(Cont_type &&cont);
        unsigned int remaining_channels();
        /* Returns number of channels left to connect */
        std::string get_channel_to_con();
    };
}