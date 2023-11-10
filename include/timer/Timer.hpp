#pragma once 

#include <boost/asio.hpp>
#include <Basic_timer.hpp>
#include <serial/Serial_context.hpp>

/// @brief Timer fireing callback after given time
/// @tparam Callb Functor with no parameters
/// @tparam Ec_callb 
template <typename Callb>
class Timer : public Basic_timer, public Serial_context
{
    /* Callback */
    Callb callb;

    /* Timer */
    boost::asio::basic_waitable_timer timer{*shared_io_context_};

    void callb_wrap(const boost::system::error_code& ec);

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

template <typename Callb>
void Timer<Callb>::callb_wrap(const boost::system::error_code& ec)
{
    if (ec == boost::asio::error::operation_aborted)
        return;
    else if (!ec)
        callb();
    else
        throw std::runtime_error{ec.what()};
}

template <typename Callb>
void Timer<Callb>::start() 
{
    timer.cancel();
    timer.expires_after(interval);
    timer.async_wait([](const boost::system::error_code& ec)
                     {
                        callb_wrap(ec);
                     });
}

template <typename Callb>
void Timer<Callb>::stop() 
{
    timer.close();
}

template <typename Callb, typename Time>
Timer(Time &&, Callb &&) -> Timer<Callb>;