#pragma once

#include <Notyfication_observer.hpp>
#include <Debug_job.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Interface for monitor notyfier
    class Debug_observer : public Notyfication_observer
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        template <typename Callb_t>
        Debug_observer(Callb_t &&callb);
    };

    template <typename Callb_t>
    inline Debug_observer::Debug_observer(Callb_t &&callb)
    {
        add_handler(
            Job_type::Urgent, 
            Job_policies<>::make_job_handler<Debug_job>(std::forward<Callb_t>(callb)));
    }
}