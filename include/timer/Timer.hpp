#pragma once

#include <Basic_timer_impl.hpp>

/// @brief Timer fireing callback after given time
class Timer : public Basic_timer_impl
{
    /* Command name */
    std::string cmd_name;

public:
    void start() override;

    template <typename Str,
              typename = typename std::enable_if_t<
                  !std::is_same_v<Timer,
                                  typename std::decay_t<Str>>>>
    Timer(Str &&cmd_name, std::chrono::milliseconds interval = std::chrono::seconds{10});
};

template <typename Str, typename>
Timer::Timer(Str &&cmd_name, std::chrono::milliseconds interval)
    : Basic_timer_impl{interval}, cmd_name{std::forward<Str>(cmd_name)}
{
}