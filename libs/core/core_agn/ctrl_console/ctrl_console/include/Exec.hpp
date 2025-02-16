#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <typename... Validators_t>
    class Exec 
        : public Validators_t ...
    {
        /// @brief Executes user's action
        /// @param arg
        virtual void usr_exec(std::string &&arg) const noexcept = 0;

    public:
        /// @brief Validates input and executes user's action when conditions satisfied
        /// @param arg
        virtual void exec(std::string &&arg) const;

        Exec() = default;
        Exec(Exec &&) noexcept = default;
        Exec(const Exec &) = default;
        Exec &operator=(Exec &&) noexcept = default;
        Exec &operator=(const Exec &) = default;
        virtual ~Exec() = default;
    };

    template <typename... Validators_t>
    inline void Exec<Validators_t...>::exec(std::string &&arg) const
    {
        if (Validators_t::validate(arg) && ...)
            usr_exec(std::move(arg))
    }
}