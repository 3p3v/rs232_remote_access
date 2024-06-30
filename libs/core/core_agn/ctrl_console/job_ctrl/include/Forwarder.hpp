#pragma once

#include <Ws_ext.hpp>

namespace Job_ctrl
{
    /// @brief Interface for forwarding jobs
    class Forwarder : public Ws_ext
    {
    public:
        /// @brief Construct and forward job to workers that are signed up for the job
        /// @tparam Job_t Type of job
        /// @tparam ...Args_t Types of the arguments
        /// @param ...args Arguments passed to job constructor
        template <
            typename Job_t,
            typename... Args_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Args_t &&...args);

        /// @brief Forward job to workers that are signed up for the job
        /// @param job Job forwarded
        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Job_t &&job);

        using Ws_ext::Ws_ext;
    };

    template <typename Job_t, typename>
    void Forwarder::forward_job(Job_t &&job)
    {
        ws.forward_job(std::forward<Job_t>(job));
    }

    template <
        typename Job_t,
        typename... Args_t,
        typename>
    inline void Forwarder::forward_job(Args_t &&...args)
    {
        ws.forward_job<Job_t>(std::forward<Args_t>(args)...);
    }
}