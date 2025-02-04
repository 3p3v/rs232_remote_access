#pragma once

#include <Ctrl_console.hpp>

Cmd_ctrl::Ctrl_console_dummy::Ctrl_console_dummy(Base_ctrl_console &&cc)
    : cc{std::move(cc)}
{
}

Cmd_ctrl::Ctrl_console::Ctrl_console(Base_ctrl_console &&cc)
    : Ctrl_console_dummy{std::move(cc)},
      Ctrl_con_storage{Ctrl_console_dummy::cc},
      Ctrl_con_exec{Ctrl_console_dummy::cc}
{
}

Cmd_ctrl::Ctrl_console::Ctrl_console(Ctrl_console &&cc) noexcept
    : Ctrl_console_dummy{std::move(cc)},
      Ctrl_con_storage{Ctrl_console_dummy::cc},
      Ctrl_con_exec{Ctrl_console_dummy::cc}
{
}

Cmd_ctrl::Ctrl_console::~Ctrl_console() = default;
