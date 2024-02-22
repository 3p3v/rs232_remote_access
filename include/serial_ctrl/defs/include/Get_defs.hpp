#pragma once

#include <string_view>

namespace Ip_serial
{
    class Hi_defs
    {
    public:
        static constexpr std::string_view get_info_s{"G_I"};

        static constexpr std::string_view get_baud_rate_s{"G_B"};
        static constexpr std::string_view get_parity_s{"G_P"};
        static constexpr std::string_view get_char_size_s{"G_C"};
        static constexpr std::string_view get_stop_bits_s{"G_S"};

        static constexpr std::string_view cts_set_s{"C_S"};
        static constexpr std::string_view cts_reset_s{"C_R"};
        static constexpr std::string_view rts_set_s{"R_S"};
        static constexpr std::string_view rts_reset_s{"R_R"};

        Hi_defs() = delete;
        // Hi_defs(Hi_defs &&) = default;
        // Hi_defs &operator=(Hi_defs &&) = default;
        // Hi_defs(const Hi_defs &) = default;
        // Hi_defs &operator=(const Hi_defs &) = default;
        // virtual ~Hi_defs() = 0;
    };
}