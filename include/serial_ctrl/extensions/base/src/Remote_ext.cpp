#include <Remote_ext.hpp>
#include <Restart_job.hpp>
#include <Forwarder.hpp>

using namespace Logic;

Remote_ext::~Remote_ext() = default;

void Remote_ext::restart_job()
{
    forward_job(Restart_job{});
}

Remote_ext::Device_shared_ptr Remote_ext::shared_from_this()
{
    return device_ptr.lock();
}

Remote_ext::Device_weak_ptr Logic::Remote_ext::weak_from_this() noexcept
{
    return device_ptr;
}
