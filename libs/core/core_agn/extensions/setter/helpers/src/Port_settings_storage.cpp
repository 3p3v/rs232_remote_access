#pragma once

#include <Port_settings_storage.hpp>

namespace Logic
{
    Port_settings_storage::Port_settings_storage(
        Port_settings::Baud_rate baud_rate,
        Port_settings::Parity parity,
        Port_settings::Char_size char_size,
        Port_settings::Stop_bits stop_bits)
        : baud_rate{baud_rate},
          parity{parity},
          char_size{char_size},
          stop_bits{stop_bits}
    {
    }
}