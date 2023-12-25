#pragma once

#include <Console.hpp>
#include <Ip_defs.hpp>
#include <Ip_hi.hpp>
#include <Ip_get.hpp>
#include <Ip_packet.hpp>

using namespace Cmd_ctrl;

namespace Logic
{
    class Interpreter : public Console, protected Ip_defs, protected Ip_hi, protected Ip_get, protected Ip_packet
    {
    public:
        Interpreter();
    };
}