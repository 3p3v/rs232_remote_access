#pragma once

#include <Policy.hpp>

namespace Cmd_ctrl
{

    template <typename... Args_t>
    class Exec
    {
    public:
        // template <typename... Adds_t>
        // class Addons
        // {
        // public:
        class Base_handle_intf// : public Adds_t...
        {
        public:
            virtual bool validate(const std::string &arg) const = 0;
            virtual void exec(Args_t... args) const = 0;
        };

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
                    /*  */
                    // (Addons_t::done() ...);

                    return (Policies_t::validate_t(arg) && ...);
                }
            };

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