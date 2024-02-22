#pragma once

#include <Port_settings_storage.hpp>
#include <Remote_conf_port_rec.hpp>

namespace Logic
{
    class Remote_record : virtual public Remote_conf_port_rec
    {
    public:
        static constexpr all_established{4};

        /// @brief Configurable device?
        bool settings_known{true};
        Port_settings_storage port_settings{};
        unsigned char params_established{};
    }; 
}