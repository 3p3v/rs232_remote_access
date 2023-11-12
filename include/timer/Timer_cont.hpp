#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <Timer.hpp>
#include <Monitor.hpp>
#include <Custom_timer.hpp>

class Timer_cont
{
    std::unordered_map<std::string, std::unique_ptr<Basic_timer>> timers;

public:
    Timer_cont() = default;
    
    template <typename Str>
    void start_timer(Str &&cmd_name);

    template <typename Str, typename Callb>
    void start_timer(Str &&cmd_name, Callb &&callb);

    void stop_timer(const std::string &cmd_name);

    void clear();
};

template <typename Str>
void Timer_cont::start_timer(Str &&cmd_name)
{
    timers.emplace(cmd_name, std::make_unique<Timer>(cmd_name)).first->second->start();
}

template <typename Str, typename Callb>
void Timer_cont::start_timer(Str &&cmd_name, Callb &&callb)
{
    timers.emplace(std::forward<Str>(cmd_name), 
                   make_custom_timer(std::forward<Callb>(callb))).first->second->start();
}