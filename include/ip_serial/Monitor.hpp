#pragma once

#include <Device.hpp>

namespace Ip_serial
{
    class Monitor
    {
    private:
        /* data */
    public:
        virtual void wake(std::shared_ptr<Device> &device) = 0;
        virtual void wake_delete(std::shared_ptr<Device> &device) = 0;
        virtual void validate(const std::string &name) = 0;

        Monitor(/* args */);
        ~Monitor();
    };
    
    
    
}