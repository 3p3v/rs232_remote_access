#pragma once

#include <string_view>

namespace Logic
{
    class Topic_defs
    {
        static constexpr std::string_view info_ch_s{"_i"};
        static constexpr std::string_view set_ch_s{"_s"};
        static constexpr std::string_view data_ch_s{"_d"};

    public:
        template <typename Str_t>
        static std::string info_ch_trans(Str_t &&dev_name);

        template <typename Str_t>
        static std::string set_ch_trans(Str_t &&dev_name);

        template <typename Str_t>
        static std::string data_ch_trans(Str_t &&dev_name);

        static constexpr unsigned char qos{0};
        static constexpr std::string_view packet_num_s{"P"};

        ~Topic_defs() = delete;
    };

    template <typename Str_t>
    inline std::string Topic_defs::info_ch_trans(Str_t &&dev_name)
    {
        return dev_name + info_ch_s;
    }

    template <typename Str_t>
    inline std::string Topic_defs::set_ch_trans(Str_t &&dev_name)
    {
        return dev_name + set_ch_s;
    }

    template <typename Str_t>
    inline std::string Topic_defs::data_ch_trans(Str_t &&dev_name)
    {
        return dev_name + data_ch_s;
    }
}