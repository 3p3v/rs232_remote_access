#pragma once

#include <string>

namespace Impl
{
    /// @brief Information required to connect to local serial port
    class Serial_info
    {
    public:
        /// @brief Serial port number
        std::string port;

        template <typename Str_t>
        Serial_info(Str_t &&port);
        Serial_info(Serial_info&&) = default;
        Serial_info& operator=(Serial_info&&) = default;
        Serial_info(const Serial_info&) = default;
        Serial_info& operator=(const Serial_info&) = default;
        virtual ~Serial_info() = 0;
    };

    template <typename Str_t>
    inline Serial_info::Serial_info(Str_t &&port)
        : port{std::forward<Str_t>(port)}
    {
    }
}