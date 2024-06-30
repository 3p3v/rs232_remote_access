#pragma once

#include <map>
#include <memory>
#include <Worker.hpp>

namespace Job_ctrl
{
    /// @brief Used for storing Workers // TODO move forwarding and storage definitions to corresponding classes & create second version that uses shared_ptr workers
    class Worker_storage
    {
        std::multimap<Worker::Job_info, Worker&> workers;

        bool is_same_worker(const Worker &w0, const Worker &w1);

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

        /// @brief Add new worker
        /// @param worker
        void add_worker(Worker &worker);

        /// @brief Remove worker
        /// @param worker
        void remove_worker(const Worker &worker);

        Worker_storage() = default;
        Worker_storage(const Worker_storage &) = delete;
        Worker_storage &operator=(const Worker_storage &) = delete;
        Worker_storage(Worker_storage &&) = default;
        Worker_storage &operator=(Worker_storage &&) = default;
        virtual ~Worker_storage() = default;
    };

    template <typename Job_t, typename>    
    void Worker_storage::forward_job(Job_t &&job)
    {
        auto job_id = job.get_id();
        auto man_iter = workers.find(job_id);

        auto r = std::all_of( // save to variable to suppress nodiscarad warning
            man_iter,
            workers.end(),
            [&job, job_id](auto &man)
            {
                if (man.first == job_id)
                {
                    man.second.give_job(job);

                    return true;
                }
                else
                {
                    return false;
                }
            });
    }

    template <typename Job_t, typename... Args_t, typename>
    inline void Worker_storage::forward_job(Args_t &&...args)
    {
        forward_job(Job_t{std::forward<Args_t>(args)...})
    }
}