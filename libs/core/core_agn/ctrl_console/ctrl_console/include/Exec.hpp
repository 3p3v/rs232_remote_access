#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Base_exec
    {
    public:
        virtual bool validate(const std::string &arg) const = 0;
        virtual void exec(std::string &&arg) const = 0;

        Base_exec() = default;
        Base_exec(Base_exec &&) noexcept = default;
        Base_exec(const Base_exec &) = default;
        Base_exec &operator=(Base_exec &&) noexcept = default;
        Base_exec &operator=(const Base_exec &) = default;
        virtual ~Base_exec() = default;
    };

    template <typename... Policies_t>
    class Exec : public Base_exec
    {
    public:
        bool validate(const std::string &arg) const override final;
    };

    template <typename Handle_t, typename... Policies_t>
    class Exec_d final : public Exec<Policies_t...>
    {
        Handle_t handle;

    public:
        Exec_d(Handle_t &&handle);

        void exec(std::string &&arg) const override;
    };

    template <typename... Policies_t>
    bool Exec<Policies_t...>::validate(const std::string &arg) const
    {
        return (Policies_t::validate_t(arg) && ...);
    }

    template <typename Handle_t, typename... Policies_t>
    Exec_d<Handle_t, Policies_t...>::Exec_d(Handle_t &&handle)
        : handle{std::move(handle)}
    {
    }

    template <typename Handle_t, typename... Policies_t>
    void Exec_d<Handle_t, Policies_t...>::exec(std::string &&arg) const
    {
        handle(std::forward<std::string>(arg));
    }
}