#pragma once

#include <Job.hpp>
#include <Port_settings_storage.hpp>
#include <Change_param_job.hpp>

namespace Logic
{
    class Change_postponed_job : public Job_identity<Change_postponed_job>
    {
    public:
        Port_settings_storage port_settings{};

        Change_postponed_job(const Change_param_job &job);
    }; 

    Change_postponed_job::Change_postponed_job(const Change_param_job &job)
        : port_settings{job.port_settings}
    {
    }
}