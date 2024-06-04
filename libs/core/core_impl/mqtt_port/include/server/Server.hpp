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
    
        using Cont = Universal_opt<Option, std::string>;
        using Get_cont = const Cont;
    }
}