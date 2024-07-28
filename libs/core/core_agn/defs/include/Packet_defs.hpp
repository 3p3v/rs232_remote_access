#pragma once

#include <string_view>

namespace Logic
{
    class Packet_defs
    {
    public:
        using Val_t = char;
        using UVal_t = std::make_unsigned_t<Val_t>;

        /// @brief Minimal message id
        static constexpr Val_t min_msg_num{' '};
        /// @brief Maximum message id
        static constexpr Val_t max_msg_num{'~'};

        /// @brief Package acknowledge message
        static constexpr std::string_view packet_ack_s{"A"};
        /// @brief Package number/id indicator
        // static constexpr std::string_view packet_num_s{"P"};

        /// @brief Invalid number of message
        static constexpr std::string_view invalid_number_s{"I_N"};
        /// @brief Message received without a number
        static constexpr std::string_view no_number_s{"N_N"};

        /// @brief Number of messages after acknowledgement is required
        static constexpr UVal_t ack_after{8};
        /// @brief Maximum number of messages in memory
        static constexpr UVal_t max_saved{15};
        /// @brief Maximum number of mesages that can be not acknowledged before showing warning
        static constexpr UVal_t max_not_ack{16};

        Packet_defs() = delete;
    };
}
