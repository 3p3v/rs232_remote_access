#pragma once

#include <Console.hpp>
#include <Ip_defs.hpp>
#include <Ip_hi.hpp>
#include <Ip_get.hpp>
#include <Ip_packet.hpp>

using namespace Cmd_ctrl;

namespace Ip_serial
{
    class Ip_console : public Console, protected Ip_defs, protected Ip_hi, protected Ip_get, protected Ip_packet
    {
    public:
        Ip_console();
    };
}