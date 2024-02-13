#pragma once

#include <memory>
#include <Port_settings.hpp>

namespace Logic
{
    class Port_settings_storage;
    template <typename Job_t>
    class Job_delegator;

    class User_controller final
    {
        using Job = std::pair<Port_settings::Setting, unsigned int>;
        
        /// @brief
        std::shared_ptr<Job_delegator<Job>> delegator;

    public:
        /// @brief Current settings
        const std::shared_ptr<const Port_settings> settings;

        /// @brief
        void set_baud_rate(const unsigned int baud_rate);
        /// @brief
        void set_parity(const Port_settings::Parity parity);
        /// @brief
        void set_char_size(const unsigned int char_size);
        /// @brief
        void set_stop_bits(const Port_settings::Stop_bits stop_bits);

        template <
            typename Shared_port_settings_storage,
            typename Shared_job_delegator>
        User_controller(
            Shared_port_settings_storage &&settings,
            Shared_job_delegator &&delegator);
        User_controller(const User_controller &) = delete;
        User_controller &operator=(const User_controller &) = delete;
        User_controller(User_controller &&) = default;
        User_controller &operator=(User_controller &&) = default;
    };

    template <typename Shared_port_settings_storage, typename Shared_job_delegator>
    inline User_controller::User_controller(Shared_port_settings_storage &&settings, Shared_job_delegator &&delegator)
        : settings{std::forward<Shared_port_settings_storage>(settings)}, 
          delegator{std::forward<Shared_job_delegator>(delegator)}
    {
    }
}