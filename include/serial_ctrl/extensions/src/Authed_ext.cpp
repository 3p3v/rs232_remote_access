#include <Authed_ext.hpp>
#include <Restart_job.hpp>

namespace Logic
{
    void Authed_ext::reset_exts()
    {
        manager->forward_job(Restart_job{});
    }
    
    Authed_ext::~Authed_ext() = default;
}