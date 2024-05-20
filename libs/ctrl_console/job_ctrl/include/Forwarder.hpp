#pragma once

#include <Ws_ext.hpp>

namespace Job_ctrl
{
    class Forwarder : public Ws_ext
    {
    public:
        /// @brief
        /// @tparam Job_t
        /// @tparam ...Args_t
        /// @tparam
        /// @param ...args
        template <
            typename Job_t,
            typename... Args_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Args_t &&...args);

        /// @brief
        /// @param job
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