#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <int number>
    class Bigger_than final
    {
    public:
        static bool validate_t(const std::string &arg);

        Bigger_than() = delete;
    };
    template <int number>
    inline bool Bigger_than<number>::validate_t(const std::string &arg)
    {
        return std::stoi(arg) > number;
    }
}