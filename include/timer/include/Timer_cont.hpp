#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <Basic_timer.hpp>

class Timer_cont
{
    std::unordered_map<std::string, std::unique_ptr<Basic_timer>> timers;

public:
    Timer_cont() = default;

    template <
    typename Timer_t,
    typename Str,
    typename... Args_t,
    typename = std::enable_if_t<
        std::is_base_of_v<
            Basic_timer,
            Timer_t>>>
    void Timer_cont::start_timer(Str &&cmd_name, Args_t &&...args);

    template <typename Str>
    void stop_timer(const Str &cmd_name);

    void clear();
};

template <
    typename Timer_t,
    typename Str,
    typename... Args_t,
    typename>
void Timer_cont::start_timer(Str &&cmd_name, Args_t &&...args)
{
    timers.emplace(std::forward<Str>(cmd_name),
                   std::make_unique<Timer_t>(std::forward<Args_t>(args)...))
        .first->second->start();
}

template <typename Str>
void Timer_cont::stop_timer(const Str &cmd_name)
{
    timers.erase(cmd_name);
}