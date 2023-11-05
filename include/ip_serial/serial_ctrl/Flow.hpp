#pragma once

#include <Flow_set>
#include <Flow_get>

namespace Ip_serial
{
    class Console;

    class Flow
    {
        size_t data{0};

    public:
        size_t get() const;

        Flow& operator+=()(unsigned int len);
    };

    
}
