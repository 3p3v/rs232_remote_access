#pragma once

#include <string>
#include <Universal_opt.hpp>

namespace Mqtt_port
{
    namespace Server
    {
        enum class Option
        {
            ip,
            port,
            cert,
            pem
        };
    
        using Get_cont = Universal_get_opt<Option, std::string>;
        using Cont = Universal_opt<Option, std::string>;
    }
}