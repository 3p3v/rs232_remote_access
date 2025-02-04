#pragma once

#include <Base_ctrl_console.hpp>


void Cmd_ctrl::Base_ctrl_console::Exec_tuple::enable(bool enable)
{
    if (!blocked)
    {
        enabled = enable;
    }
}

void Cmd_ctrl::Base_ctrl_console::Exec_tuple::enable()
{
    if (!blocked)
    {
        enabled = true;
    }
}

void Cmd_ctrl::Base_ctrl_console::Exec_tuple::disable()
{
    if (!blocked)
    {
        enabled = false;
    }
}

Cmd_ctrl::Base_ctrl_console::Exec_tuple::Exec_tuple(std::unique_ptr<Base_exec> &&exec, bool enabled, bool blocked)
    : exec{std::move(exec)}, blocked{blocked}
{
}