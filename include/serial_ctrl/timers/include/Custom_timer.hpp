#pragma once

#include <Basic_timer_impl.hpp>

/// @brief Timer fireing callback after given time
template <typename Callb>
class Custom_timer : public Basic_timer_impl
{
    /* Callback */
    Callb callb;

public:
    void start() override;

    Custom_timer(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10});
};

template <typename Callb>
Custom_timer<Callb>::Custom_timer(Callb &&callb, std::chrono::milliseconds interval)
    : Basic_timer_impl{interval}, callb{std::move(callb)}
{
}

template <typename Callb>
void Custom_timer<Callb>::start()
{
    timer.cancel();
    timer.expires_after(interval);
    timer.async_wait([this](const boost::system::error_code& ec)
    {
        if (!ec)
            callb();
        else if (ec != boost::asio::error::operation_aborted)
            Monitor::get().error(Exception::Serial_except{ec.what()});
    });        
}

template <typename Callb>
Custom_timer(Callb&&) -> Custom_timer<Callb>;

template <typename Callb>
Custom_timer(Callb&&, std::chrono::milliseconds) -> Custom_timer<Callb>;

class Custom_timer_maker
{
public:
    template <typename Callb>
    auto make_timer(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
    {
        return std::make_unique<Custom_timer<Callb>>(std::forward<Callb>(callb), interval);
    }
};