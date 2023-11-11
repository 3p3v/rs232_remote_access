#pragma once

#include <exception>
#include <string>

// namespace Com_cli
// {
namespace Exception
{
    /// @brief Base exception class
    class Exception : public std::exception
    {
        std::string module; 
        std::string expln;

    public:
        template <typename S1, typename S2>
        Exception(S1 &&module, S2 &&expln);

        const char *what() const override;
    };

    template <typename S1, typename S2>
    Exception::Exception(S1 &&module, S2 &&expln)
        : module{std::forward<S1>(module)}, expln{std::forward<S2>(expln)}
    {
    }
}
// }