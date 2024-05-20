#pragma once

#include <Notyfication_observer.hpp>
#include <Error_job.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Interface for monitor notyfier
    class Error_observer : public Notyfication_observer
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        template <typename Callb_t>
        Error_observer(Callb_t &&callb);
    };

    template <typename Callb_t>
    inline Error_observer::Error_observer(Callb_t &&callb)
    {
        add_handler(
            Job_type::Urgent, 
            Job_policies<>::make_job_handler<Error_job>(std::forward<Callb_t>(callb)));
    }
}