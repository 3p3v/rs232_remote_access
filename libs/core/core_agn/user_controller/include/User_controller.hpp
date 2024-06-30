#pragma once

#include <memory>
#include <Port_settings_storage.hpp>
#include <Forwarder.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Remote_record;

    /// @brief Change settings of the device
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

        /// @brief 
        void run();

        /// @brief 
        void restart();

        User_controller(Forwarder &&manager);
        User_controller(const Forwarder &manager);
    };

    template <typename... Params_t>
    inline void User_controller::set_params(Params_t... params)
    {
        manager.forward_job(Change_param_job{Port_settings_storage{params...}});
    }
}