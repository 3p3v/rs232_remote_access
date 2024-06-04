#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Is_cmd final
    {
    public:
        static bool validate_t(const std::string &arg);

        Is_cmd() = delete;
    };
}