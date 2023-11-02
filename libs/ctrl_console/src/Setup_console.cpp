#pragma once

#include <stdexcept>
#include <Setup_console.hpp>

namespace Cmd_ctrl
{
    namespace Setup
    {
        void Setup_console::check()
        {
            std::for_each(cmds.begin(), cmds.end(), [](auto &cmd)
                          {
                    if (!cmd.second->check())
                    {
                        throw std::runtime_error("Value: " + cmd.first + " was not given!");
                    } });
        }

    }
}