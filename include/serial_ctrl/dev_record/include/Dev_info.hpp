#pragma once

#include <Remote_dev.hpp>
#include <Serial_port_record.hpp>
#include <Remote_port_record.hpp>

namespace Logic
{
    /// @brief Full device record (device name (MQTT topic), serial port name, parameters and status)
    class Remote_full
        : public Remote_dev,
          public Serial_port_record,
          public Remote_port_record
    {
    public:
        template <
            typename Str1_t,
            typename Str2_t>
        Remote_full(
            Str1_t &&remote_name,
            Str2_t &&port_name);

        template <
            typename Str1_t,
            typename Str2_t>
        Remote_full(
            Str1_t &&remote_name,
            Str2_t &&port_name,
            Remote_conf_port conf_port);

        template <
            typename Str1_t,
            typename Str2_t,
            typename... Params_t>
        Remote_full(
            Str1_t &&remote_name,
            Str2_t &&port_name,
            Params_t... params,
            Remote_conf_port conf_port);

        template <
            typename Str1_t,
            typename Str2_t,
            typename... Params_t>
        Remote_full(
            Str1_t &&remote_name,
            Str2_t &&port_name,
            Params_t... params);
    };

    template <typename Str1_t, typename Str2_t>
    inline Remote_full::Remote_full(Str1_t &&remote_name, Str2_t &&port_name)
        : Remote_port_record{std::forward<Str1_t>(remote_name)},
          Serial_port_record{std::forward<Str2_t>(port_name)}
    {
    }

    template <
        typename Str1_t,
        typename Str2_t>
    Remote_full::Remote_full(
        Str1_t &&remote_name,
        Str2_t &&port_name,
        Remote_conf_port conf_port)
        : Remote_port_record{std::forward<Str1_t>(remote_name)},
          Serial_port_record{std::forward<Str2_t>(port_name)},
          Remote_dev{conf_port}
    {
    }

    template <
        typename Str1_t,
        typename Str2_t,
        typename... Params_t>
    inline Remote_full::Remote_full(
        Str1_t &&remote_name,
        Str2_t &&port_name,
        Params_t... params,
        Remote_conf_port conf_port)
        : Remote_port_record{std::forward<Str1_t>(remote_name)},
          Serial_port_record{std::forward<Str2_t>(port_name)},
          Remote_dev{params..., conf_port}
    {
    }

    template <
        typename Str1_t,
        typename Str2_t,
        typename... Params_t>
    inline Remote_full::Remote_full(
        Str1_t &&remote_name,
        Str2_t &&port_name,
        Params_t... params)
        : Remote_port_record{std::forward<Str1_t>(remote_name)},
          Serial_port_record{std::forward<Str2_t>(port_name)},
          Remote_dev(params...)
    {
    }
}