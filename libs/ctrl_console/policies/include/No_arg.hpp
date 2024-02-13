#pragma once

#include <string>

namespace Cmd_ctrl
{
    class No_arg final
    {
    public:
        static bool validate_t(const std::string &arg);

        No_arg() = delete;
    };
}