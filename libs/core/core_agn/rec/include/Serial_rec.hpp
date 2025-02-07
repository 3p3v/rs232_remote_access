#pragma once

#include <string>

namespace Logic
{
    /// @brief Information required to connect to local serial port
    class Serial_rec
    {
    public:
        /// @brief Serial port number
        std::string port;

        template <typename Str_t>
        Serial_rec(Str_t &&port);
    };

    template <typename Str_t>
    inline Serial_rec::Serial_rec(Str_t &&port)
        : port{std::forward<Str_t>(port)}
    {
    }
}