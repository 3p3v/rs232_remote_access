#pragma once

#include <vector>

namespace Mqtt_port
{   
    class Executor
    {
    public:
        using Data_type = std::vector<unsigned char>;

    public:
        virtual void exec(const Data_type &data) = 0;
    };
}