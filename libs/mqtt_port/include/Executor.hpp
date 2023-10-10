#pragma once

#include <vector>

namespace Mqtt_port
{   
    class Executor
    {
    public:
        using Data = std::vector<unsigned char>;

    public:
        virtual void exec(const Data &data) = 0;
    };
}