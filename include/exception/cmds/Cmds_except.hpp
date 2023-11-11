#pragma once

#include <Exception.hpp>

namespace Exception
{
    class Cmds_except : public Exception
    {
    public:
        Cmds_except(const std::string &expln);
        Cmds_except(const char *expln);
    };
}