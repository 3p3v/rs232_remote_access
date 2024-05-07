#pragma once

#include <Process_manager.hpp>
#include <Ctrl_con_exec.hpp>
#include <Ext_forwarder.hpp>

namespace Logic
{
    using Console_exec = Ctrl_con_exec<Con_storage_param, Endl_opt::with>;
    
    /// @brief All classes needed for communication linked into one to save on creating shared_ptrs
    class Process_full
        : public Process_manager,
          public Console_exec,
          public Ext_forwarder
    {
    };
}