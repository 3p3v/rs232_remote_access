#pragma once

namespace Cmd_ctrl
{
    /// @brief Set parameters used by command type
    /// @tparam ...Args_t 
    template <typename... Args_t>
    class Exec
    {
    public:
        class Base_handle_intf
        {
        public:
            virtual bool validate(const std::string &arg) const = 0;
            virtual void exec(Args_t... args) const = 0;
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
                Base_handle()
                {
                }

                bool validate(const std::string &arg) const
                {
                    return (Policies_t::validate_t(arg) && ...);
                }
            };

            /// @brief Set handler used when command was succesfully validated
            /// @tparam Handle_f handler
            template <typename Handle_f>
            class Dyn_handle : public Base_handle
            {
                Handle_f handle;

            public:
                Dyn_handle(Handle_f &&handle)
                    : handle{std::forward<Handle_f>(handle)}
                {
                }

                void exec(Args_t... args) const
                {
                    handle(std::forward<Args_t>(args)...);
                }
            };

            template <typename Handle_f>
            Dyn_handle(Handle_f &&) -> Dyn_handle<Handle_f>;
        };
        // };
    };
}