#pragma once

#include <vector>
#include <array>

namespace Serial_port
{
    class Data_interface
    {
    public:
        using Data = std::array<unsigned char, 1300>;//std::vector<unsigned char>;
    // public:
    //     virtual void write(const Data &data) = 0;
    };
}