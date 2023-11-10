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
// }