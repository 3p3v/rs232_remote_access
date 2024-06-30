#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <Basic_timer.hpp>

/// @brief Collection of timers
class Timer_cont
{
    std::unordered_map<std::string, std::unique_ptr<Basic_timer>> timers;

public:
    Timer_cont() = default;

    /// @brief Consrtuct ant start new timer
    /// @tparam Timer_t Timer implementation
    /// @tparam Str Timer name type
    /// @tparam ...Args_t Types of the arguments
    /// @param cmd_name Timer name 
    /// @param ...args Arguments passed to constructor
    template <
        typename Timer_t,
        typename Str,
        typename... Args_t,
        typename = std::enable_if_t<
            std::is_base_of_v<
                Basic_timer,
                std::decay_t<Timer_t>>>>
    void start_timer(Str &&cmd_name, Args_t &&...args);

    /// @brief Start new timer
    /// @tparam Timer_t 
    /// @tparam Str 
    /// @param cmd_name Timer name 
    /// @param timer Timer implementation
    template <
        typename Timer_t,
        typename Str,
        std::enable_if_t<
            std::is_base_of_v<
                Basic_timer,
                std::decay_t<Timer_t>>,
            bool> = true>
    void start_timer(Str &&cmd_name, Timer_t &&timer);

    /// @brief Start new timer
    /// @tparam Str 
    /// @tparam Timer_ptr_t 
    /// @tparam  
    /// @param cmd_name Timer name 
    /// @param timer Timer unique_ptr implementation
    template <
        typename Str,
        typename Timer_ptr_t,
        std::enable_if_t<
            std::is_base_of_v<
                Basic_timer,
                typename Timer_ptr_t::element_type>,
            bool> = true>
    void start_timer(Str &&cmd_name, Timer_ptr_t &&timer);

    /// @brief Stop timer with given name
    /// @tparam Str Sequence of signs type
    /// @param name Timer name
    template <typename Str>
    void stop_timer(const Str &name);

    /// @brief Kill all timers
    void clear();
};

template <
    typename Timer_t,
    typename Str,
    typename... Args_t,
    typename>
void Timer_cont::start_timer(Str &&cmd_name, Args_t &&...args)
{
    timers.emplace(
              std::forward<Str>(cmd_name),
              std::make_unique<Timer_t>(std::forward<Args_t>(args)...))
        .first->second->start();
}

template <
    typename Timer_t,
    typename Str,
    std::enable_if_t<
        std::is_base_of_v<
            Basic_timer,
            std::decay_t<Timer_t>>,
        bool>>
void Timer_cont::start_timer(Str &&cmd_name, Timer_t &&timer)
{
    timers.emplace(
              std::forward<Str>(cmd_name),
              std::make_unique<Timer_t>(std::forward<Timer_t>(timer)))
        .first->second->start();
}

template <
    typename Str,
    typename Timer_ptr_t,
    std::enable_if_t<
        std::is_base_of_v<
            Basic_timer,
            typename Timer_ptr_t::element_type>,
        bool>>
void Timer_cont::start_timer(Str &&cmd_name, Timer_ptr_t &&timer)
{
    timers.emplace(
              std::forward<Str>(cmd_name),
              std::forward<Timer_ptr_t>(timer))
        .first->second->start();
}

template <typename Str>
void Timer_cont::stop_timer(const Str &cmd_name)
{
    timers.erase(cmd_name);
}