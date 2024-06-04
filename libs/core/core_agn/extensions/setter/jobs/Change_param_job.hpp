#pragma once

#include <Job.hpp>
#include <Port_settings_storage.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Change_param_job : public Job_identity<Change_param_job>
    {
    public:
        Port_settings_storage port_settings{};

        template <
            typename Port_settings_storage_t,
            typename = std::enable_if_t<
                std::is_same_v<
                    Port_settings_storage,
                    std::decay_t<Port_settings_storage_t>>>>
        Change_param_job(Port_settings_storage_t &&port_settings);
    };

    template <typename Port_settings_storage_t, typename>
    inline Change_param_job::Change_param_job(Port_settings_storage_t &&port_settings)
        : port_settings{std::forward<Port_settings_storage_t>(port_settings)}
    {
    }
}