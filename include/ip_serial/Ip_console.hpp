#pragma once

#include <Console.hpp>
#include <Ip_defs.hpp>

using namespace Cmd_ctrl;

namespace Ip_serial
{
    class Ip_console : public Console, public Ip_defs
    {
    public:
        Ip_console();
    };
}