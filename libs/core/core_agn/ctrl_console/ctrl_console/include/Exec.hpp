#pragma once

namespace Cmd_ctrl
{
    /// @brief Set parameters used by command type
    /// @tparam ...Args_t
    template <typename... Args_t>
    class Exec
    {
    public:
        template <typename Arg>
        class Param
        {
        public:
            class Base_handle_intf
            {
            public:
                virtual bool validate(const Arg &arg) const = 0;
                virtual void exec(Args_t... args, Arg arg) const = 0;
            };

            /// @brief Set policies checking validity of received argument
            /// @tparam ...Policies_t Classes implementing "static bool validate_t(const std::string &arg)" method
            template <typename... Policies_t>
            class Policies
            {
            public:
                class Base_handle : public Base_handle_intf
                {
                public:
                    /// @brief Argument type for handler
                    using Arg = Arg;

                    bool validate(const Arg &arg) const override final
                    {
                        return (Policies_t::validate_t(arg) && ...);
                    }
                };

                /// @brief Defines handler used when command was succesfully validated
                /// @tparam Handle_f handler
                template <typename Handle_f>
                class Dyn_handle final : public Base_handle
                {
                    Handle_f handle;

                public:
                    Dyn_handle(Handle_f &&handle)
                        : handle{std::move(handle)}
                    {
                    }

                    void exec(Args_t... args, Arg arg) const override
                    {
                        handle(std::forward<Args_t>(args)..., std::forward<Arg>(arg));
                    }
                };

                template <typename Handle_f>
                Dyn_handle(Handle_f &&) -> Dyn_handle<Handle_f>;
            };
        };
    };
}