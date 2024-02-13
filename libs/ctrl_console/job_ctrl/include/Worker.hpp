#pragma once

#include <unordered_map>
#include <typeinfo>
#include <algorithm>
#include <any>
#include <queue>
#include <tuple>
#include <memory>
#include <stdexcept>
#include <Exec.hpp>
#include <Job_handler.hpp>

namespace Job_ctrl
{
    enum class Job_type : bool
    {
        Urgent = true,
        Trivial = false
    };
    
    class Worker
    {
    public:
        using Job_info = Job::Job_id;

    private:
        using Job_queue = std::queue<std::unique_ptr<Job>>;
        using Job_handler_intf_ptr = std::unique_ptr<Job_handler_intf>;
        using Job_tuple = std::tuple<Job_type, Job_handler_intf_ptr, Job_queue>;
        using Job_map = std::unordered_map<Job_info, Job_tuple>;

        /// @brief Maps job id to handler
        Job_map job_handlers;
        /// @brief All jobs to do
        std::queue<Job_map::iterator> jobs;

        decltype(auto) get_type(Job_tuple& t);
        decltype(auto) get_handler(Job_tuple& t);
        decltype(auto) get_queue(Job_tuple& t);

    protected:
        template <
            typename Job_handler_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job_handler_intf,
                    Job_handler_t>>>
        void add_handler(Job_type mandatority, Job_handler_t &&handler);

        size_t take_job();

    public:
        std::vector<Job_info> get_ids();

        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    Job_t>>>
        void give_job(Job_t &&job);
        

        Worker() = default;
        Worker(const Worker &) = delete;
        Worker& operator=(const Worker &) = delete;
        Worker(Worker &&) = default;
        Worker& operator=(Worker &&) = default;
        virtual ~Worker() = 0;
    };

    inline decltype(auto) Worker::get_type(Job_tuple &t)
    {
        return std::get<0>(t);
    }

    inline decltype(auto) Worker::get_handler(Job_tuple &t)
    {
        return std::get<1>(t);
    }

    inline decltype(auto) Worker::get_queue(Job_tuple &t)
    {
        return std::get<2>(t);
    }

    template <typename Job_handler_t, typename>
    inline void Worker::add_handler(Job_type run_type, Job_handler_t &&handler)
    {
        job_handlers.emplace(
                Job_handler_t::s_get_id(), 
                std::make_tuple(
                    run_type, 
                    std::make_unique<Job_handler_t>(
                            std::forward<Job_handler_t>(handler)), 
                    Job_queue{}));
    }

    template <typename Job_t, typename>
    inline void Worker::give_job(Job_t &&job)
    {
        auto job_h = job_handlers.find(job.get_id());
        
        if (job_h != job_handlers.end())
        {
            if (get_type(job_h->second) == Job_type::Urgent)
            {
                get_handler(job_h->second)->exec(job);
            }
            else
            {
                get_queue(job_h->second).emplace(std::make_unique<std::decay_t<Job_t>>(std::forward<Job_t>(job)));
                jobs.push(job_h);
            }
        }
        else
        {
            throw std::logic_error{"Wrong job number!"};
        }
    }
}