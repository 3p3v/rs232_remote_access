#pragma once

#include <Notyfication_forwarder.hpp>

namespace Logic
{
    class Monitor_dummy : public Notyfication_forwarder
    {
    public:
        void error(Dev_num dev, const std::exception &exc) override;
        void debug(Dev_num dev, const std::exception &exc) override;
    };    
}