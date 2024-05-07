#include <Remote_ext.hpp>
#include <Restart_job.hpp>
#include <Ext_forwarder.hpp>

using namespace Logic;

Remote_ext::~Remote_ext() = default;

void Remote_ext::restart_job()
{
    forward_job(Restart_job{});
}

// Remote_ext::Remote_ext(Forwarder_ptr &&forwarder)
//     : forwarder{std::move(forwarder)}
// {
// }

// Remote_ext::Remote_ext(const Forwarder_ptr &forwarder)
//     : forwarder{forwarder}
// {
// }