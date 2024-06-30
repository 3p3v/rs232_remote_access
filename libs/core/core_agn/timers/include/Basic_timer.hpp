#pragma once

#include <chrono>

/// @brief Interface for timer class (non-implementation)
class Basic_timer
{
protected:
    const std::chrono::milliseconds interval;

public:
    /// @brief Start the timer
    virtual void start() = 0;
    /// @brief Stop the timer
    virtual void stop() = 0;

    template <typename Time>
    Basic_timer(Time &&interval);
    Basic_timer(Basic_timer&&) = default;
    Basic_timer& operator=(Basic_timer&&) = default;
    Basic_timer(Basic_timer&) = delete;
    Basic_timer& operator=(Basic_timer&) = delete;
    virtual ~Basic_timer() = default;
};

template <typename Time>
Basic_timer::Basic_timer(Time &&interval)
    : interval{std::forward<Time>(interval)}
{
}
