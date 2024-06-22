#pragma once

#include <string>
#include <Topic_defs.hpp>

namespace Impl
{
    /// @brief Information required to connect to remote
    class Remote_info
    {
    public:
        const std::string info_ch;
        const std::string set_ch;
        const std::string data_ch;
        const std::string name;

        template <typename Str>
        Remote_info(Str &&name);
        Remote_info(Remote_info&&) = default;
        Remote_info& operator=(Remote_info&&) = default;
        Remote_info(const Remote_info&) = delete;
        Remote_info& operator=(const Remote_info&) = delete;
        virtual ~Remote_info() = 0;
    };

    template <typename Str>
    inline Remote_info::Remote_info(Str &&name)
        : info_ch{Topic_defs::info_ch_trans(name)},
          set_ch{Topic_defs::set_ch_trans(name)},
          data_ch{Topic_defs::data_ch_trans(std::forward<Str>(name))},
          name{std::forward<Str>(name)}
    {
    }
}
