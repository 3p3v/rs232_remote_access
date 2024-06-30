#pragma once

#include <string_view>

namespace Logic
{
    class Disconnect_defs
    {
    public:
        /// @brief Device disconnected 
        static constexpr std::string_view disconne_detect_s{"D_D"};

        Disconnect_defs() = delete;
    };
}