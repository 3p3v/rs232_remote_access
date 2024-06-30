#pragma once

#include <string_view>

namespace Logic
{
    class Topic_defs
    {
        /// @brief Information topic suffix
        static constexpr std::string_view info_ch_s{"i"};
        /// @brief Set topic suffix
        static constexpr std::string_view set_ch_s{"s"};
        /// @brief Data topic suffix
        static constexpr std::string_view data_ch_s{"d"};

    public:
        /// @brief Transform MAC to information topic
        /// @tparam Str_t 
        /// @param dev_name Devices MAC
        /// @return Topic name
        template <typename Str_t>
        static std::string info_ch_trans(Str_t &&dev_name);

        /// @brief Transform MAC to set topic
        /// @tparam Str_t 
        /// @param dev_name Devices MAC
        /// @return Topic name
        template <typename Str_t>
        static std::string set_ch_trans(Str_t &&dev_name);

        /// @brief Transform MAC to data topic
        /// @tparam Str_t 
        /// @param dev_name Devices MAC
        /// @return Topic name
        template <typename Str_t>
        static std::string data_ch_trans(Str_t &&dev_name);

        /// @brief Package number/id indicator
        static constexpr std::string_view packet_num_s{"P"};

        Topic_defs() = delete;
    };

    template <typename Str_t>
    inline std::string Topic_defs::info_ch_trans(Str_t &&dev_name)
    {
        return std::forward<Str_t>(dev_name) + std::string{info_ch_s};
    }

    template <typename Str_t>
    inline std::string Topic_defs::set_ch_trans(Str_t &&dev_name)
    {
        return std::forward<Str_t>(dev_name) + std::string{set_ch_s};
    }

    template <typename Str_t>
    inline std::string Topic_defs::data_ch_trans(Str_t &&dev_name)
    {
        return std::forward<Str_t>(dev_name) + std::string{data_ch_s};
    }
}