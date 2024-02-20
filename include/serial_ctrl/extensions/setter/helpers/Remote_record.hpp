#pragma once

#include <Port_settings_storage.hpp>

namespace Logic
{
    class Remote_record
    {
    public:
        static constexpr all_established{4};

        /// @brief Configurable device?
        bool conf_port{true};
        /// @brief Configurable device?
        bool settings_known{true};
        Port_settings_storage port_settings{};
        unsigned char params_established{};
    }; 
}