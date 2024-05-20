#include <Debug_monitor.hpp>
#include <iostream>

void Logic::Debug_monitor::cout_debug(Dev_num num, const std::string &info)
{
    std::cout << info << '\n';
}

Logic::Debug_monitor::Debug_monitor()
    : Debug_observer{
        [this](Debug_job &job)
        { 
            cout_debug(job.num, job.info); 
        }}
{
}