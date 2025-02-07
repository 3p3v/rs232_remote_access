#pragma once

#include <string>
#include <Topic_defs.hpp>

namespace Logic
{
    /// @brief Information required to connect to remote
    class Remote_rec
    {
    public:
        /// @brief Device's info topic name
        const std::string info_ch;
        /// @brief Device's set topic name
        const std::string set_ch;
        /// @brief Device's data topic name
        const std::string data_ch;
        /// @brief Device's MAC
        const std::string name;

        template <typename Str>
        Remote_rec(Str &&name);
    };

    template <typename Str>
    inline Remote_rec::Remote_rec(Str &&name)
        : info_ch{Topic_defs::info_ch_trans(name)},
          set_ch{Topic_defs::set_ch_trans(name)},
          data_ch{Topic_defs::data_ch_trans(std::forward<Str>(name))},
          name{std::forward<Str>(name)}
    {
    }
}
