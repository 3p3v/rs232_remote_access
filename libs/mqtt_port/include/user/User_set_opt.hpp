#pragma once

#include <User_opt.hpp>

namespace Mqtt_port
{
    class User_set_opt
    {
    public:
        virtual void set_options(User_opt &user_opt) = 0;
    };
}