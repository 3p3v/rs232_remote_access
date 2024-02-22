#pragma once

#include <Job.hpp>
#include <Port_settings_storage.hpp>

namespace Logic
{
    class Change_param_job : public Job_identity<Change_param_job>
    {
    public:
        Port_settings_storage port_settings{};
    }; 
}