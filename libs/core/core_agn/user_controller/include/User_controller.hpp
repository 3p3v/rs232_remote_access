#pragma once

#include <memory>
#include <Port_settings_storage.hpp>
#include <Forwarder.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Remote_record;

    /// @brief User controlling interface, can change devices settings, ect.
    class User_controller final
    {
        /// @brief
        Forwarder manager;

    public:
        /// @brief Set all params at once
        /// @param port_settings
        void set_params(Port_settings_storage &&port_settings);

        /// @brief Set all params at once
        /// @param port_settings
        void set_params(const Port_settings_storage &port_settings);

        template <
            typename... Params_t>
        void set_params(Params_t... params);

        /// @brief Run the device
        void run();

        /// @brief Restart the device
        void restart();

        /// @brief Construct user controlling interface
        /// @param manager Device's message forwarding mechanism
        User_controller(Forwarder &&manager);
        
        /// @brief Construct user controlling interface
        /// @param manager Device's message forwarding mechanism
        User_controller(const Forwarder &manager);
    };

    template <typename... Params_t>
    inline void User_controller::set_params(Params_t... params)
    {
        manager.forward_job(Change_param_job{Port_settings_storage{params...}});
    }
}