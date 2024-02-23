#pragma once

#include <memory>
#include <Remote_record.hpp>
#include <Process_manager.hpp>

namespace Logic
{
    class Port_settings_storage;
    template <typename Job_t>
    class Job_delegator;

    class User_controller final
    {   
        /// @brief
        std::shared_ptr<Ext_forwarder> manager;

    public:
        /// @brief Current settings
        const std::shared_ptr<Remote_record> settings;

        // /// @brief
        // void set_baud_rate(const unsigned int baud_rate);
        // /// @brief
        // void set_parity(const Port_settings::Parity parity);
        // /// @brief
        // void set_char_size(const unsigned int char_size);
        // /// @brief
        // void set_stop_bits(const Port_settings::Stop_bits stop_bits);

        template <
            typename Port_settings_storage_t,
            typename = std::enable_if_t<
                std::is_same_v<
                    Port_settings_storage,
                    std::decay_t<Port_settings_storage_t>>>>
        void set_params(Port_settings_storage_t &&port_settings);

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

    template <typename Port_settings_storage_t, typename>
    inline void User_controller::set_params(Port_settings_storage_t &&port_settings)
    {
        manager->forward_job(Change_param_job{std::forward<Port_settings_storage_t>(port_settings)});
    }

    template <typename Shared_port_settings_storage, typename Shared_job_delegator>
    inline User_controller::User_controller(Shared_port_settings_storage &&settings, Shared_job_delegator &&delegator)
        : settings{std::forward<Shared_port_settings_storage>(settings)},
          manager{std::forward<Shared_job_delegator>(delegator)}
    {
    }
}