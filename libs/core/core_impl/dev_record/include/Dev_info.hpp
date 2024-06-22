#pragma once

#include <Serial_info.hpp>
#include <Remote_info.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Full device record (device name (MQTT topic), serial port name, parameters and status)
    class Dev_info
        : public Serial_info,
          public Remote_info
    {
    public:
        template <
            typename Str1_t,
            typename Str2_t>
        Dev_info(
            Str1_t &&remote_name,
            Str2_t &&port_name);
    };

    template <typename Str1_t, typename Str2_t>
    inline Dev_info::Dev_info(Str1_t &&remote_name, Str2_t &&port_name)
        : Remote_info{std::forward<Str1_t>(remote_name)},
          Serial_info{std::forward<Str2_t>(port_name)}
    {
    }
}