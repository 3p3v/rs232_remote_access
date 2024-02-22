#include <Unauthed_ext.hpp>

namespace Logic
{
    void Unauthed_ext::forward_job(Job &&job)
    {
        if (auto manager_ = manager.lock())
        {
            manager_->forward_job(std::move(Restart_job{}));
        }
    }

    void Unauthed_ext::forward_job(const Job &job)
    {
        if (auto manager_ = manager.lock())
        {
            manager_->forward_job(job);
        }
    }

    UnUnauthed_ext::~UnUnauthed_ext() = default;
}