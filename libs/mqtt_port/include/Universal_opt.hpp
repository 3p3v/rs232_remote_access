#pragma once

#include <Universal_get_opt.hpp>

namespace Mqtt_port
{
    template <tyename Class_opt, typename Val_type>
    class Universal_opt : public Universal_get_opt<Class_opt, Val_type>
    {
    public:
        Universal_opt(/* args */);
        ~Universal_opt();
    };
    
    Universal_opt::Universal_opt(/* args */)
    {
    }
    
    Universal_opt::~Universal_opt()
    {
    }
    
}