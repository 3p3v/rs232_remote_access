#pragma once

#include <string_view>

namespace Ip_serial
{
    class Ip_hi
    {
    public:
        static constexpr std::string_view master_hi_s{"M_HI"};
        static constexpr std::string_view slave_hi_s{"S_HI"};
        static constexpr std::string_view master_keep_alive_s{"M_KA"};
        static constexpr std::string_view slave_keep_alive_s{"S_KA"};

        Ip_hi() = default;
        Ip_hi(Ip_hi &&) = default;
        Ip_hi &operator=(Ip_hi &&) = default;
        Ip_hi(const Ip_hi &) = default;
        Ip_hi &operator=(const Ip_hi &) = default;
        virtual ~Ip_hi() = 0;
    };
}