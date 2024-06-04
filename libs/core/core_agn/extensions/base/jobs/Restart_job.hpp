#pragma once

#include <Job.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Job for restarting all modules
    class Restart_job : public Job_identity<Restart_job>
    {
    };
}