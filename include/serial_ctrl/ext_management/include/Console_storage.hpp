#pragma once

#include <Remote_ext.hpp>
#include <Ctrl_con_storage.hpp>

using namespace Cmd_ctrl;

namespace Logic
{
    /// @brief Argument types for received command
    using Con_storage_param = Remote_ext::Command;
    using Console_storage_base = Ctrl_con_storage<Con_storage_param, Endl_opt::with>;

    class Console_storage : public Console_storage_base
    {
        using Console_storage_base::Console_storage_base;
    };
}