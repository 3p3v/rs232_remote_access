#pragma once

#include <memory>
#include <Port_settings_storage.hpp>

namespace Logic
{
    class Remote_record;
    class Ext_forwarder;

    class User_controller final
    {
    public:
        using Ext_forwarder_ptr = std::shared_ptr<Ext_forwarder>;
    private:
        /// @brief
        Ext_forwarder_ptr manager;

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

        User_controller(Ext_forwarder_ptr &&manager);
        User_controller(const Ext_forwarder_ptr &manager);

        User_controller(const User_controller &) = delete;
        User_controller &operator=(const User_controller &) = delete;
        User_controller(User_controller &&) = default;
        User_controller &operator=(User_controller &&) = default;
    };

    template <typename... Params_t>
    inline void User_controller::set_params(Params_t... params)
    {
        manager->forward_job(Change_param_job{Port_settings_storage{params...}});
    }
}