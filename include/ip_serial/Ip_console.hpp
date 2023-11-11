#pragma once

#include <Console.hpp>
#include <Ip_defs.hpp>

using namespace Cmd_ctrl;

namespace Ip_serial
{
    class Ip_console : public Console, protected Ip_defs, protected Ip_hi
    {
    public:
        Ip_console();
    };
}