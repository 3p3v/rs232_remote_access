#include <Debug_monitor.hpp>
#include <iostream>

void Logic::Debug_monitor::cout_debug(Dev_num num, const std::string &info)
{
    if (debug)
    {
        std::clog << "Device " << std::to_string(num) << ":";
        std::cout << info << '\n';
    }
}

Logic::Debug_monitor::Debug_monitor(bool debug)
    : Debug_observer{
        [this](Debug_job &job)
        { 
            cout_debug(job.num, job.info); 
        }},
      debug{debug}
{
}