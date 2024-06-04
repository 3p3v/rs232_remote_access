#pragma once

#include <Port_settings.hpp>

namespace Logic
{
    class Port_settings_storage
    {
    public:
        /// @brief
        Port_settings::Baud_rate baud_rate{9600};
        /// @brief
        Port_settings::Parity parity{Port_settings::Parity::none};
        /// @brief
        Port_settings::Char_size char_size{8};
        /// @brief
        Port_settings::Stop_bits stop_bits{Port_settings::Stop_bits::one};

        Port_settings_storage() = default;
        Port_settings_storage(
            Port_settings::Baud_rate baud_rate,
            Port_settings::Parity parity,
            Port_settings::Char_size char_size,
            Port_settings::Stop_bits stop_bits);
    };

}