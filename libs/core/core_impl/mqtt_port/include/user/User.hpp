#pragma once

#include <string>
#include <Universal_opt.hpp>

namespace Mqtt_port
{
    namespace User
    {
        enum class Option
        {
            id,
            no_clean,
            username,
            password
        };
    
        using Cont = Universal_opt<Option, std::string>;
        using Get_cont = const Cont;
    }
}