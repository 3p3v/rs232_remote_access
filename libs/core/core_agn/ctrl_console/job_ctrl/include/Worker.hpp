#pragma once

#include <unordered_map>
#include <typeinfo>
#include <algorithm>
#include <any>
#include <deque>
#include <queue>
#include <tuple>
#include <memory>
#include <string>
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

    enum class Job_excluded : bool
    {
        Only_private_queue = true,
        Main_queue = false
    };

    class Worker
    {
    public:
        using Job_info = Job::Job_id;
        // using Job_done_remaining = std::pair<size_t, size_t>;

    private:
        using Job_queue = std::deque<std::unique_ptr<Job>>;
        using Job_handler_intf_ptr = std::unique_ptr<Job_handler_intf>;
        using Job_tuple = std::tuple<Job_type, Job_handler_intf_ptr, Job_queue, Job_excluded>;
        using Job_map = std::unordered_map<Job_info, Job_tuple>;

        /// @brief Maps job id to handler
        Job_map job_handlers;
        /// @brief All jobs to do
        std::queue<Job_map::iterator> jobs;

        decltype(auto) get_type(Job_tuple &t);
        decltype(auto) get_handler(Job_tuple &t);
        decltype(auto) get_queue(Job_tuple &t);
        decltype(auto) get_excluded(Job_tuple &t);

    protected:
        template <
            typename Job_handler_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job_handler_intf,
                    std::decay_t<Job_handler_t>>>>
        void add_handler(Job_type mandatority, Job_handler_t &&handler);

        template <
            typename Job_handler_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job_handler_intf,
                    std::decay_t<Job_handler_t>>>>
        void add_handler(Job_type mandatority, Job_excluded excluded, Job_handler_t &&handler);

        template <
            typename Job_handler_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job_handler_intf,
                    std::decay_t<Job_handler_t>>>>
        void override_handler(Job_type mandatority, Job_handler_t &&handler);

        template <
            typename Job_handler_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job_handler_intf,
                    std::decay_t<Job_handler_t>>>>
        void override_handler(Job_type mandatority, Job_excluded excluded, Job_handler_t &&handler);

        size_t check_for_jobs();

        template <typename Job_t>
        size_t check_for_jobs();

        size_t take_job();

        template <typename Job_t>
        size_t take_job();

        /// @brief Remove all jobs of some kind
        /// @tparam Job_t 
        /// @return 
        template <typename Job_t>
        size_t remove_jobs();

        /// @brief Take most recent job of some kind, delete rest
        /// @tparam Job_t 
        /// @return 
        template <typename Job_t>
        size_t take_most_recent_job_and_clear();

    public:
        std::vector<Job_info> get_ids() const;

        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void give_job(Job_t &&job);

        Worker() = default;
        Worker(const Worker &) = delete;
        Worker &operator=(const Worker &) = delete;
        Worker(Worker &&) = default;
        Worker &operator=(Worker &&) = default;
        virtual ~Worker() = 0;
    };

    inline decltype(auto) Worker::get_type(Job_tuple &t)
    {
        return std::get<0>(t);
    }

    inline decltype(auto) Worker::get_excluded(Job_tuple &t)
    {
        return std::get<3>(t);
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
        add_handler(run_type, Job_excluded::Main_queue, std::forward<Job_handler_t>(handler));
    }

    template <typename Job_handler_t, typename>
    inline void Worker::add_handler(Job_type run_type, Job_excluded excluded, Job_handler_t &&handler)
    {
        auto id = Job_handler_t::s_get_id();

        if (job_handlers.find(id) == job_handlers.end())
        {
            job_handlers.emplace(
                id,
                std::make_tuple(
                    run_type,
                    std::make_unique<Job_handler_t>(
                        std::forward<Job_handler_t>(handler)),
                    Job_queue{},
                    excluded));
        }
        else
        {
            throw std::logic_error{"Handler with key: " + std::to_string(id) + " already exists!"};
        }
    }

    template <typename Job_handler_t, typename>
    inline void Worker::override_handler(Job_type run_type, Job_handler_t &&handler)
    {
        override_handler(run_type, Job_excluded::Main_queue, std::forward<Job_handler_t>(handler));
    }

    template <typename Job_handler_t, typename>
    inline void Worker::override_handler(Job_type run_type, Job_excluded excluded, Job_handler_t &&handler)
    {
        auto id = Job_handler_t::s_get_id();

        auto job_k = job_handlers.find(id);

        if (job_k != job_handlers.end())
        {
            job_k->second = std::make_tuple(
                run_type,
                std::make_unique<Job_handler_t>(
                    std::forward<Job_handler_t>(handler)),
                Job_queue{},
                excluded);
        }
        else
        {
            throw std::logic_error{"Cannot override handler with key: " + std::to_string(id) + " becaurse it does not exist!"};
        }
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
                get_queue(job_h->second).push_front(std::make_unique<std::decay_t<Job_t>>(std::forward<Job_t>(job)));

                if (get_excluded(job_h->second) == Job_excluded::Main_queue)
                    jobs.push(job_h);
            }
        }
        else
        {
            throw std::logic_error{"No job: " + std::string{typeid(Job_t).name()} + " registered!"};
        }
    }

    template <typename Job_t>
    size_t Worker::check_for_jobs()
    {
        /* Get job id */
        auto job_id = Job_t::s_get_id();

        /* Find requested job */
        auto job_h = job_handlers.find(job_id);

        if (job_h != job_handlers.end())
        {
            if (get_excluded(job_h->second) == Job_excluded::Only_private_queue)
            {
                return get_queue(job_h->second).size();
            }
            else
            {
                throw std::logic_error{"Job: " + std::string{typeid(Job_t).name()} + " was not excluded from the main queue!"};
            }
        }
        else
        {
            throw std::logic_error{"No job: " + std::string{typeid(Job_t).name()} + " registered!"};
        }
    }

    template <typename Job_t>
    size_t Worker::take_job()
    {
        /* Get job id */
        auto job_id = Job_t::s_get_id();

        /* Find requested job */
        auto job_h = job_handlers.find(job_id);

        if (job_h != job_handlers.end())
        {
            if (get_excluded(job_h->second) == Job_excluded::Only_private_queue)
            {
                if (get_queue(job_h->second).size())
                {
                    /* Do job */
                    get_handler(job_h->second)->exec(*get_queue(job_h->second).front());
                    /* Delete job from queue */
                    get_queue(job_h->second).pop_front();

                    return get_queue(job_h->second).size();
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                throw std::logic_error{"Job: " + std::string{typeid(Job_t).name()} + " was not excluded from the main queue!"};
            }
        }
        else
        {
            throw std::logic_error{"No job: " + std::string{typeid(Job_t).name()} + " registered!"};
        }
    }

    template <typename Job_t>
    inline size_t Worker::remove_jobs()
    {
        /* Get job id */
        auto job_id = Job_t::s_get_id();

        /* Find requested job */
        auto job_h = job_handlers.find(job_id);

        if (job_h != job_handlers.end())
        {
            if (get_excluded(job_h->second) == Job_excluded::Only_private_queue)
            {
                get_queue(job_h->second).clear();
                
                return 0;
            }
            else
            {
                throw std::logic_error{"Job: " + std::string{typeid(Job_t).name()} + " was not excluded from the main queue!"};
            }
        }
        else
        {
            throw std::logic_error{"No job: " + std::string{typeid(Job_t).name()} + " registered!"};
        }
    }

    template <typename Job_t>
    inline size_t Worker::take_most_recent_job_and_clear()
    {
        /* Get job id */
        auto job_id = Job_t::s_get_id();

        /* Find requested job */
        auto job_h = job_handlers.find(job_id);

        if (job_h != job_handlers.end())
        {
            if (get_excluded(job_h->second) == Job_excluded::Only_private_queue)
            {
                if (get_queue(job_h->second).size())
                {
                    /* Do job */
                    get_handler(job_h->second)->exec(*get_queue(job_h->second).back());
                    /* Delete job from queue */
                    get_queue(job_h->second).clear();

                    return 0;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                throw std::logic_error{"Job: " + std::string{typeid(Job_t).name()} + " was not excluded from the main queue!"};
            }
        }
        else
        {
            throw std::logic_error{"No job: " + std::string{typeid(Job_t).name()} + " registered!"};
        }
    }
}