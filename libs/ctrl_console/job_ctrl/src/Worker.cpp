#pragma once

#include <Worker.hpp>

namespace Job_ctrl
{
    std::vector<Worker::Job_info> Worker::get_ids() const
    {
        auto ids = std::vector<Job_info>(job_handlers.size());
        std::for_each(
            ids.begin(),
            ids.end(),
            [job_iter = job_handlers.begin()](auto &id) mutable
            {
                id = job_iter->first;
                job_iter++;
            });

        return ids;
    }

    size_t Worker::check_for_jobs()
    {
        return jobs.size();
    }

    size_t Worker::take_job()
    {
        size_t done{};
        
        if (jobs.size())
        {
            /* Do job */
            get_handler(jobs.front()->second)->exec(*(get_queue(jobs.front()->second).front()));
            /* Delete job */
            get_queue(jobs.front()->second).pop();
            jobs.pop();

            done++;
        }

        /* Return number of jobs left */
        return jobs.size();
    }

    Worker::~Worker() = default;
}