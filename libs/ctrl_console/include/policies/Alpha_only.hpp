#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Alpha_only final
    {
    public:
        static bool validate_t(const std::string &arg);

        Alpha_only() = delete;
    };
}