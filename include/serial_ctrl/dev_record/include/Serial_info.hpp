#pragma once

#include <string>

namespace Logic
{
    /// @brief Information required to connect to local serial port
    class Serial_port_record
    {
    public:
        /// @brief Serial port number
        std::string port;

        template <typename Str_t>
        Serial_port_record(Str_t &&port);
        Serial_port_record(Serial_port_record&&) = default;
        Serial_port_record& operator=(Serial_port_record&&) = default;
        Serial_port_record(const Serial_port_record&) = delete;
        Serial_port_record& operator=(const Serial_port_record&) = delete;
        virtual ~Serial_port_record() = 0;
    };

    template <typename Str_t>
    inline Serial_port_record::Serial_port_record(Str_t &&port)
        : port{std::forward<Str_t>(port)}
    {
    }
}