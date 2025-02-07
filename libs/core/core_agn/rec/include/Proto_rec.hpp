#pragma once

#include <Port_settings_rec.hpp>

namespace Logic
{
    class Proto_rec
    {
    public:
        enum class Remote_conf_port : bool
        {
            Configurable = true,
            Non_configurable = false
        };

        enum class Remote_status
        {
            /// @brief Not tried to connect yet
            Not_connected,
            /// @brief Choosing port parameters
            Establishing_parameters,
            /// @brief Ready to exchange data
            Data_exchange,
            /// @brief Got disconnected
            Disconnected
        };

        static constexpr auto all_established{4};
        unsigned char params_established{};
        bool record_active{false};
        Remote_status status{Remote_status::Not_connected};

        const Remote_conf_port conf_port{Remote_conf_port::Configurable};
        bool settings_known{true};
        Port_settings_rec port_settings{};

        Proto_rec(
            Remote_conf_port conf_port,
            bool settings_known,
            Port_settings_rec &&port_settings
        );
    };
}