#pragma once

#include <memory>
#include <Port_settings_storage.hpp>
#include <Forwarder.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Remote_record;

    class User_controller final
    {
        /// @brief
        Forwarder manager;

    public:
        // /// @brief
        // /// @param baud_rate
        // void set_baud_rate(Port_settings::Baud_rate baud_rate);

        // /// @brief
        // /// @param parity
        // void set_parity(Port_settings::Parity parity);

        // /// @brief
        // /// @param char_size
        // void set_char_size(Port_settings::Char_size char_size);

        // /// @brief
        // /// @param stop_bits
        // void set_stop_bits(Port_settings::Stop_bits stop_bits);

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