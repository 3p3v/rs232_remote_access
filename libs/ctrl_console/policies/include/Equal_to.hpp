#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <typename Val_t, Val_t number>
    class Equal_to final
    {
    public:
        static bool validate_t(const std::string &arg);
        static bool validate_t(const char *arg);
        static bool validate_t(unsigned int arg);

        Equal_to() = delete;
    };
    
    template <typename Val_t, Val_t number>
    inline bool Equal_to<Val_t, number>::validate_t(const std::string &arg)
    {
        return std::stoi(arg) == number;
    }

    template <typename Val_t, Val_t number>
    inline bool Equal_to<Val_t, number>::validate_t(const char *arg)
    {
        return std::stoi(arg) == number;
    }

    template <typename Val_t, Val_t number>
    inline bool Equal_to<Val_t, number>::validate_t(unsigned int arg)
    {
        return arg == number;
    }
}