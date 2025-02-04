#pragma once

#include <Ctrl_con_storage.hpp>

void Cmd_ctrl::Ctrl_con_storage::disable_all() noexcept
{
    std::for_each(
        cc.cmds.begin(),
        cc.cmds.end(),
        [](auto &c)
        {
            c.second.disable();
        }
    );
}

Cmd_ctrl::Ctrl_con_storage::Ctrl_con_storage(Base_ctrl_console &cc)
    : cc{cc}
{
}
