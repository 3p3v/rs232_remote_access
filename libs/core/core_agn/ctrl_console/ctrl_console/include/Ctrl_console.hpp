#pragma once

#include <Ctrl_con_storage.hpp>
#include <Ctrl_con_exec.hpp>

namespace Cmd_ctrl
{
    class Ctrl_console_dummy
    {
    public:
        Base_ctrl_console cc;

        Ctrl_console_dummy(Base_ctrl_console &&cc);
    };
    
    /// @brief Wrapper for all console functionality
    class Ctrl_console
        : private Ctrl_console_dummy,
          public Ctrl_con_storage,
          public Ctrl_con_exec
    {
    public:
        Ctrl_console(Base_ctrl_console &&cc);
        Ctrl_console(Ctrl_console &&) noexcept;
        Ctrl_console& operator=(Ctrl_console &&) = delete;
        Ctrl_console(const Ctrl_console &) = delete;
        Ctrl_console& operator=(const Ctrl_console &) = delete;
        ~Ctrl_console();
    };
}