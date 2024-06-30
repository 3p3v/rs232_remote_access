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
        /// @brief Construct record
        /// @param conf_port If port is configurable
        Remote_dev(Remote_conf_port conf_port);

        /// @brief Construct record
        /// @param port_settings Baud rate, char size, ect.
        /// @param conf_port If port is configurable
        Remote_dev(Port_settings_storage &&port_settings, Remote_conf_port conf_port);

        /// @brief Construct port, pass settings directly
        /// @tparam ...Params_t 
        /// @param ...params Settings
        /// @param conf_port If port is configurable
        template <typename... Params_t>
        Remote_dev(
            Params_t... params,
            Remote_conf_port conf_port);

        /// @brief Construct port, pass settings directly
        /// @tparam ...Params_t 
        /// @param ...params Settings
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