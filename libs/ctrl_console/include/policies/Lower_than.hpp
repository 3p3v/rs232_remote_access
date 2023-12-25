#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <int number>
    class Lower_than final
    {
    public:
        static bool validate_t(const std::string &arg);

        Lower_than() = delete;
    };
    template <int number>
    inline bool Lower_than<number>::validate_t(const std::string &arg)
    {
        return std::stoi(arg) < number;
    }
}