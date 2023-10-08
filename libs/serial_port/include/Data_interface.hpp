#pragma once

#include <vector>

namespace Serial_port
{
    class Data_interface
    {
    public:
        using Data_type = unsigned char;
        using Cont_type = std::vector<Data_type>;
    public:
        virtual void write(const Cont_type &data) = 0;
    };
}
