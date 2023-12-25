#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <int number>
    class Equal_to final
    {
    public:
        static bool validate_t(const std::string &arg);

        Equal_to() = delete;
    };
    template <int number>
    inline bool Equal_to<number>::validate_t(const std::string &arg)
    {
        return std::stoi(arg) == number;
    }
}