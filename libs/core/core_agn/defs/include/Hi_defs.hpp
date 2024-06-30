#pragma once

#include <string_view>

namespace Logic
{
    class Hi_defs
    {
    public:
        /// @brief Welcoming the device
        static constexpr std::string_view master_hi_s{"M_HI"};
        /// @brief Welcoming response
        static constexpr std::string_view slave_hi_s{"S_HI"};
        /// @brief Ping device
        static constexpr std::string_view master_keep_alive_s{"M_KA"};
        /// @brief Ping back from device
        static constexpr std::string_view slave_keep_alive_s{"S_KA"};

        Hi_defs() = delete;
    };
}