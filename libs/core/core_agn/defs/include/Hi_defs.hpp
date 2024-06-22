#pragma once

#include <string_view>

namespace Logic
{
    class Hi_defs
    {
    public:
        static constexpr std::string_view master_hi_s{"M_HI"};
        static constexpr std::string_view slave_hi_s{"S_HI"};
        static constexpr std::string_view master_keep_alive_s{"M_KA"};
        static constexpr std::string_view slave_keep_alive_s{"S_KA"};

        Hi_defs() = delete;
    };
}