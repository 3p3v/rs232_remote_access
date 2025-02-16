#pragma once 

#include <Exec.hpp>

namespace Cmd_ctrl
{
    template <typename Exec_t, typename... Policies_t>
    class Policy_exec : public Exec_t
    {
    public:
        void exec(std::string &&arg) const override final;
    };

    template <typename Exec_t, typename... Policies_t>
    void Policy_exec<Exec_t, Policies_t...>::exec(std::string &&arg) const
    {
        if (Policies_t::validate_t(arg) && ...)
            Exec_t::usr_exec(std::move(arg))
    }
} // namespace Cmd_ctrl
