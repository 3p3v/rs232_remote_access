#pragma once

#include <exception>
#include <string>

namespace Logic
{
    /// @brief Base extension exception class
    class Intf_except : public std::exception
    {
        const std::string expln;

    public:
        Intf_except(const std::string &expln);

        Intf_except(const char *expln);

        Intf_except(const std::string &expln, int code);

        Intf_except(const char *expln, int code);

        explicit Intf_except(int code);

        const char *what() const noexcept override;
    };
}
// }