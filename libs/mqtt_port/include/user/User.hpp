#pragma once

#include <string>
#include <Universal_opt.hpp>

namespace Mqtt_port
{
    namespace User
    {
        enum class Option
        {
            no_clean,
            username,
            password
        };
    
        using Get_cont = Universal_get_opt<Option, std::string>;
        using Cont = Universal_opt<Option, std::string>;
    }
}