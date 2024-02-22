#include <Common_ext.hpp>

namespace Logic
{
    void Common_ext::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this]()
                {
                    timers.clear();
                }));
    }

    Common_ext::~Common_ext() = default;
}