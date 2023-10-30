#pragma once

#include <vector>

namespace Serial_port
{
    class Data_interface
    {
    public:
        using Data = std::vector<unsigned char>;
    // public:
    //     virtual void write(const Data &data) = 0;
    };
}
