#pragma once

#include <Process_manager.hpp>

namespace Logic
{
    /// @brief All classes needed for communication linked into one to save on creating shared_ptrs
    class Process_full
        : public Process_manager,
          public Console_exec
    {
    };
}