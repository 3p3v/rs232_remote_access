#pragma once

#include <string_view>

namespace Logic
{
    class Get_defs
    {
    public:
        /// @brief Get devices current settings
        static constexpr std::string_view get_info_s{"G_I"};

        /// @brief Get devices baud rate
        static constexpr std::string_view get_baud_rate_s{"G_B"};
        /// @brief Get devices parity
        static constexpr std::string_view get_parity_s{"G_P"};
        /// @brief Get devices char size
        static constexpr std::string_view get_char_size_s{"G_C"};
        /// @brief Get devices current settstop bitsings
        static constexpr std::string_view get_stop_bits_s{"G_S"};

        static constexpr std::string_view cts_set_s{"C_S"};
        static constexpr std::string_view cts_reset_s{"C_R"};
        static constexpr std::string_view rts_set_s{"R_S"};
        static constexpr std::string_view rts_reset_s{"R_R"};

        Get_defs() = delete;
    };
}