#include <Debug_monitor.hpp>
#include <iostream>

void Impl::Debug_monitor::cout_debug(Dev_num num, const std::string &info)
{
    if (debug)
    {
        std::lock_guard<std::mutex> write_guard{write_mutex};

        std::clog << "[Device: " << infos.infos[num].name << "]\n";
        std::cout << info << '\n\n';
    }
}

Impl::Debug_monitor::Debug_monitor(Impl_cont &infos, bool debug)
    : Debug_observer{
        [this](Debug_job &job)
        { 
            cout_debug(job.num, job.info); 
        }},
      debug{debug},
      infos{infos}
{
}