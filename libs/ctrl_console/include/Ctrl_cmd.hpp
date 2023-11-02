#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Ctrl_cmd
    {
    public:
        std::string name{};
        std::string arg{};

        template <typename Str>
        Ctrl_cmd(Str &&name);

        template <typename Iter>
        Ctrl_cmd(Iter begin, Iter end);

        template <typename Iter>
        Ctrl_cmd(Iter begin, Iter end, Iter a_begin, Iter a_end);
    };

    template <typename Str>
    Ctrl_cmd::Ctrl_cmd(Str &&name)
        : name{std::forward<Str>(name)}
    {
    }

    template <typename Iter>
    Ctrl_cmd::Ctrl_cmd(Iter begin, Iter end)
        : name{begin, end}
    {
    }

    template <typename Iter>
    Ctrl_cmd::Ctrl_cmd(Iter begin, Iter end, Iter a_begin, Iter a_end)
        : name{begin, end}, arg{a_begin, a_end}
    {
    }
}