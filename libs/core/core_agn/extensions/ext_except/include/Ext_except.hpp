#pragma once

#include <exception>
#include <string>

namespace Logic
{
    /// @brief Base extension exception class
    class Ext_except : public std::exception
    {
        const std::string expln;

    public:
        template <typename Str_t>
        Ext_except(Str_t &&expln);

        const char *what() const noexcept override;
    };

    template <typename Str_t>
    Ext_except::Ext_except(Str_t &&expln)
        : expln{std::forward<Str_t>(expln)}
    {
    }
}
// }