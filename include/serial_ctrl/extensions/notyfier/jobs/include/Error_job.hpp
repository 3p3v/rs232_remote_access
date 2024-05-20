#pragma once

#include <Job.hpp>
#include <Notyfier_defs.hpp>
#include <stdexcept>

using namespace Job_ctrl;

namespace Logic
{
    class Error_job : public Job_identity<Error_job>
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        Dev_num num;

        const std::exception& exc;

        Error_job(Dev_num num, const std::exception& exc);
    };
}