#pragma once 

#include <boost/asio.hpp>
#include <Basic_timer.hpp>

/// @brief Timer fireing callback after given time
/// @tparam Callb Functor with no parameters
/// @tparam Ec_callb 
template <typename Callb>
class Timer : public Basic_timer
{
    Callb callb;

public:
    void start() override;
    void stop() override;

    template <typename Time>
    Timer(Time &&interval, Callb &&callb);
};

template <typename Callb>
template <typename Time>
Timer<Callb>::Timer(Time &&interval, Callb &&callb)
    : Basic_timer{std::forward<Time>(interval)}, callb{std::move(callb)}
{
}

template <typename Callb, typename Time>
Timer(Time &&, Callb &&) -> Timer<Callb>;