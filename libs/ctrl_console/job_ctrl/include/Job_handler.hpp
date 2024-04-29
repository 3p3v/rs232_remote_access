#pragma once

// #include <any>
#include <Exec.hpp>
#include <Job.hpp>

using namespace Cmd_ctrl;

namespace Job_ctrl
{
    using Job_handler_param = Exec<>::Param<Job &>;

    using Job_handler_intf = Job_handler_param::Base_handle_intf;

    template <typename... Policies_t>
    class Job_policies : Job_handler_param::Policies<Policies_t...>
    {
    public:
        /// @brief Defines handler used when command was succesfully validated
        /// @tparam Job_t Derived type that argument is casted to
        /// @tparam Handler_t handler
        template <
            typename Job_t,
            typename Handler_t,
            std::enable_if_t<
                std::is_base_of_v<
                    std::decay_t<Job>,
                    std::decay_t<Job_t>>,
                bool> = true>
        class Job_handler final : public Base_handle
        {
            Handler_t handler;

        public:
            Job_handler(Handler_t &&handler)
                : handler{std::move(handler)}
            {
            }

            /// @brief Use when job type is not known
            /// @param arg 
            void exec(Job &arg) const override
            {
                handler(dynamic_cast<Job_t &>(arg));
            }

            /// @brief Use when job type is known to avoid dynamic casting
            /// @param arg 
            void exec(Job_t &arg)
            {
                handler(arg);
            }

            Job::Job_id get_id()
            {
                return Job_t::s_get_id();
            }

            static Job::Job_id s_get_id()
            {
                return Job_t::s_get_id();
            }
        };

        template <typename Job_t, typename Handler_t>
        static auto make_job_handler(Handler_t &&handler)
        {
            return Job_handler<Job_t, Handler_t>{std::forward<Handler_t>(handler)};
        }
    };
}