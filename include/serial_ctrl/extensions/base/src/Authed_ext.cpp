#include <Authed_ext.hpp>
#include <Restart_job.hpp>

namespace Logic
{
    void Authed_ext::forward_job(Job &&job)
    {
        if (auto manager_ = manager.lock())
        {
            manager_->forward_job(std::move(Restart_job{}));
        }
    }

    void Authed_ext::forward_job(const Job &job)
    {
        if (auto manager_ = manager.lock())
        {
            manager_->forward_job(job);
        }
    }
    
    Authed_ext::~Authed_ext() = default;
}