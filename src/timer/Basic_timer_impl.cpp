#include <Basic_timer_impl.hpp>

void Basic_timer_impl::stop() 
{
    timer.cancel();
}

Basic_timer_impl::Basic_timer_impl(std::chrono::milliseconds interval)
    : Basic_timer{std::chrono::seconds{interval}}
{
}

Basic_timer_impl::~Basic_timer_impl()
{
    stop();
}