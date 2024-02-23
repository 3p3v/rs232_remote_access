#pragma once

#include <Ctrl_con_storage.hpp>
#include <Ctrl_con_exec.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_console
        : public Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>,
          public Ctrl_con_exec<Base_handle_t, endl_opt, endl_, space_>
    {
    };
}