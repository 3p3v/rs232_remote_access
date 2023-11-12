#include <Timer_cont.hpp>

void Timer_cont::stop_timer(const std::string &cmd_name)
{
    timers.erase(cmd_name);
}
