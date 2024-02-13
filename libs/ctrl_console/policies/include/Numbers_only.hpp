#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Numbers_only final
    {
    public:
        static bool validate_t(const std::string &arg);

        Numbers_only() = delete;
    };
}