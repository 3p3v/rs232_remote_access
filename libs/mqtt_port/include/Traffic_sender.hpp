#pragma once

#include <string>
#include <vector>

namespace Mqtt_port
{
    class Traffic_sender
    {
    public:
        using Data = std::vector<unsigned char>;

    public:
        virtual void write(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;
    };
}