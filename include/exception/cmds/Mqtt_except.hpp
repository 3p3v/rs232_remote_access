#pragma once

#include <Exception.hpp>

namespace Exception
{
    /// @brief Base exception class
    class Mqtt_unknown : public Exception
    {
        std::string expln;

    public:
        template <typename Str>
        Exception(Str &&expln);

        const char *what() const override;
    };

    template <typename Str>
    Exception::Exception(Str &&expln)
        : expln{std::forward<Str>(expln)}
    {
    }

}
