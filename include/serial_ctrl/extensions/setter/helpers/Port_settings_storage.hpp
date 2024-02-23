#pragma once

#include <Port_settings.hpp>

namespace Logic
{
    class Port_settings_storage
    {
    public:
        /// @brief
        unsigned int baud_rate{9600};
        /// @brief
        Port_settings::Parity parity{Port_settings::Parity::none};
        /// @brief
        unsigned int char_size{8};
        /// @brief
        Port_settings::Stop_bits stop_bits{Port_settings::Stop_bits::one};
    };
    
}