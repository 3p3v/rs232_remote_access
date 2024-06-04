#pragma once

#include <Remote_record.hpp>
#include <Remote_status_record.hpp>

namespace Logic
{
    /// @brief Full device record (device name (MQTT topic), serial port name, parameters and status)
    class Remote_dev
        : public Remote_record,
          public Remote_status_record
    {
    public:
        Remote_dev(Remote_conf_port conf_port);

        Remote_dev(Port_settings_storage &&port_settings, Remote_conf_port conf_port);

        template <typename... Params_t>
        Remote_dev(
            Params_t... params,
            Remote_conf_port conf_port);

        template <typename... Params_t>
        Remote_dev(
            Params_t... params);
    };

    template <typename... Params_t>
    inline Remote_dev::Remote_dev(
        Params_t... params,
        Remote_conf_port conf_port)
        : Remote_record(params...),
          Remote_conf_port_rec{conf_port}
    {
    }

    template <typename... Params_t>
    inline Remote_dev::Remote_dev(Params_t... params)
        : Remote_record(params...)
    {
    }
}