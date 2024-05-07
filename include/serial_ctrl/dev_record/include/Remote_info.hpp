#pragma once

#include <string>
#include <Topic_info.hpp>

namespace Logic
{
    /// @brief Information required to connect to remote
    class Remote_port_record
    {
    public:
        const std::string info_ch;
        const std::string set_ch;
        const std::string data_ch;

        template <typename Str>
        Remote_port_record(Str &&name);
        Remote_port_record(Remote_port_record&&) = default;
        Remote_port_record& operator=(Remote_port_record&&) = default;
        Remote_port_record(const Remote_port_record&) = delete;
        Remote_port_record& operator=(const Remote_port_record&) = delete;
        virtual ~Remote_port_record() = 0;
    };

    template <typename Str>
    inline Remote_port_record::Remote_port_record(Str &&name)
        : info_ch{Topic_info::info_ch_trans(name)},
          set_ch{Topic_info::set_ch_trans(name)},
          data_ch{Topic_info::data_ch_trans(std::forward<Str>(name))}
    {
    }
}
