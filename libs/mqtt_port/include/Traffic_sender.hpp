#pragma once

#include <string>
#include <vector>

namespace Mqtt_port
{
    class Traffic_sender
    {
    public:
        using Data_type = std::vector<unsigned char>;

    public:
        virtual void write(const std::string &channel_name, const Data_type &data) = 0;
    };
}