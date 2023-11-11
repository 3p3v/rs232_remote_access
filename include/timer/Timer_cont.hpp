#pragma once

#include <unordered_map>
#include <string>
#include <Timer.hpp>
#include <Monitor.hpp>

class Timer_cont
{
    std::unordered_map<std::string, Timer> timers;
    Monitor &monitor;

public:
    Timer_cont(Monitor &monitor);
    
    template <typename Str>
    void start_timer(Str &&cmd_name);

    void stop_timer(const std::string &cmd_name);
};

template <typename Str>
void Timer_cont::start_timer(Str &&cmd_name)
{
    timers.emplace(cmd_name, Timer{cmd_name, monitor}).first->second.start();
}