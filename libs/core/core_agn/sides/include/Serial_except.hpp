#pragma once

#include <Intf_except.hpp>

namespace Logic
{
    /// @brief Serial exception class
    class Serial_except : public Intf_except
    {
        using Intf_except::Intf_except;

    public:
        template <typename Str_t>
        Serial_except(Str_t &&expln, int code);

        explicit Serial_except(int code);
    };

    template <typename Str_t>
    inline Serial_except::Serial_except(Str_t &&expln, int code)
        : Intf_except{std::forward<Str_t>(expln) + std::string{' '} + std::to_string(code)}
    {
    }

}
// }