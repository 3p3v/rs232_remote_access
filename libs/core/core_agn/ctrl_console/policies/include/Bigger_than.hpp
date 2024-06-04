#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <typename Val_t, Val_t number>
    class Bigger_than final
    {
    public:
        static bool validate_t(const std::string &arg);
        static bool validate_t(const char *arg);
        static bool validate_t(Val_t arg);

        Bigger_than() = delete;
    };

    template <typename Val_t, Val_t number>
    inline bool Bigger_than<Val_t, number>::validate_t(const std::string &arg)
    {
        return std::stoi(arg) > number;
    }

    template <typename Val_t, Val_t number>
    inline bool Bigger_than<Val_t, number>::validate_t(const char *arg)
    {
        return std::stoi(arg) > number;
    }

    template <typename Val_t, Val_t number>
    inline bool Bigger_than<Val_t, number>::validate_t(Val_t arg)
    {
        return arg > number;
    }
}