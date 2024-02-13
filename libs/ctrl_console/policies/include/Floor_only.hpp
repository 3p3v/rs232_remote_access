#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Floor_only final
    {
    public:
        static bool validate_t(const std::string &arg);

        Floor_only() = delete;
    };
}