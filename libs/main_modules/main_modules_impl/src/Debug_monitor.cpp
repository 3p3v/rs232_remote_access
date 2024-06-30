#include <Debug_monitor.hpp>
#include <iostream>

void Impl::Debug_monitor::cout_debug(Dev_num num, const std::string &info)
{
    if (debug)
    {
        auto [info_, lock] = infos.get_info_and_lock(num);

        std::clog << "[Device: " << info_.name << "]\n";
        std::cout << info << '\n\n';
    }
}

Impl::Debug_monitor::Debug_monitor(Info_getter &&infos, bool debug)
    : Debug_observer{
        [this](Debug_job &job)
        { 
            cout_debug(job.num, job.info); 
        }},
      debug{debug},
      infos{std::move(infos)}
{
}