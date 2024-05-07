#pragma once

// #include <map>
// #include <memory>
#include <Worker_storage.hpp>

namespace Job_ctrl
{
    template <typename Worker_ptr_t>
    class Forwarder : virtual public Worker_storage<Worker_ptr_t>
    {
    public:
        /// @brief Forward job to Workers taking some kind of Job
        /// @tparam Job_t
        /// @param job
        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Job_t &&job); // TODO change from template to normal, add forward_job<Job_t>(args...)

        template <
            typename Job_t,
            typename ...Args_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Args_t &&...args);

        Forwarder() = default;
        Forwarder(Forwarder&&) = default;
        Forwarder& operator=(Forwarder&&) = default;
        Forwarder(const Forwarder&) = default;
        Forwarder& operator=(const Forwarder&) = default;
        ~Forwarder() = 0;
    };

    template <
        typename Worker_ptr_t>
    template <
        typename Job_t,
        typename>
    inline void Forwarder<Worker_ptr_t>::forward_job(Job_t &&job)
    {
        auto job_id = job.get_id();
        auto man_iter = workers.find(job_id);

        std::all_of(
            man_iter,
            workers.end(),
            [&job, job_id](auto &man)
            {
                if (man.first == job_id)
                {
                    if constexpr (std::is_pointer_v<Worker_ptr_t> || is_shared_ptr<Worker_ptr_t>::value || is_unique_ptr<Worker_ptr_t>::value)
                    {
                        man.second->give_job(std::forward<Job_t>(job));
                    }
                    else
                    {
                        man.second.give_job(std::forward<Job_t>(job));
                    }

                    return true;
                }
                else
                {
                    return false;
                }
            });
    }

    template <typename Worker_ptr_t>
    template <typename Job_t, typename ...Args_t, typename>
    inline void Forwarder<Worker_ptr_t>::forward_job(Args_t &&...args)
    {
        forward_job(Job_t{std::forward<Args_t>(args)...})
    }

    template <typename Worker_ptr_t>
    Forwarder<Worker_ptr_t>::~Forwarder() = default;
}