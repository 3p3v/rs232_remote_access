#pragma once

#include <Debug_observer.hpp>

namespace Logic
{
    class Debug_monitor : public Debug_observer
    {
        void cout_debug(Dev_num num, const std::string& info);

    public:
        Debug_monitor();
    };
}