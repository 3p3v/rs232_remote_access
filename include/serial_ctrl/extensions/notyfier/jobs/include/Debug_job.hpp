#pragma once

#include <Job.hpp>
#include <Notyfier_defs.hpp>
#include <string>

using namespace Job_ctrl;

namespace Logic
{
    class Debug_job : public Job_identity<Debug_job>
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        Dev_num num;

        const std::string& info;

        Debug_job(Dev_num num, const std::string& info);
    };
}