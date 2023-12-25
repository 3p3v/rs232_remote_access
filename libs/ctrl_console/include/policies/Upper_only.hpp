#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Upper_only final
    {
    public:
        static bool validate_t(const std::string &arg);

        Upper_only() = delete;
    };
}