#pragma once

#include <memory>
#include <Basic_timer.hpp>
#include <Monitor.hpp>

template <typename Callb>
class Periodic_timer : public Basic_timer_impl
{
    std::unique_ptr<Basic_timer> cmd_timer;
    Callb callb;

public:
    template <typename Timer_t>
    Periodic_timer(Callb &&callb, Timer_t &&cmd_timer, std::chrono::milliseconds interval = std::chrono::seconds{60});
    ~Periodic_timer();

    void start() override;
    void stop() override;
};

template <typename Callb>
template <typename Timer_t>
Periodic_timer<Callb>::Periodic_timer(Callb &&callb, Timer_t &&cmd_timer, std::chrono::milliseconds interval)
    : Basic_timer_impl{interval},
      callb{std::move(callb)},
      cmd_timer{std::make_unique<Timer_t>(std::forward<Timer_t>(cmd_timer))}
{
}

template <typename Callb>
Periodic_timer<Callb>::~Periodic_timer()
{
    stop();
}

template <typename Callb>
void Periodic_timer<Callb>::start()
{
    cmd_timer->stop();
    timer.cancel();
    timer.expires_after(interval);
    timer.async_wait([this](const boost::system::error_code &ec)
                     {
                        if (!ec)
                        {
                            /* Run callback */
                            callb();
                            /* Restart timer */
                            start();
                            /* Run command timer */
                            cmd_timer->start(); 
                        }
                        else if (ec != boost::asio::error::operation_aborted)
                        {
                            Monitor::get().error(Exception::Serial_except{ec.what()});
                        } });
}

template <typename Callb>
void Periodic_timer<Callb>::stop()
{
    cmd_timer->stop();
    timer.cancel();
}

template <typename Callb, typename Timer_t>
Periodic_timer(Callb&&, Timer_t&&) -> Periodic_timer<Callb>;

template <typename Callb, typename Timer_t>
Periodic_timer(Callb&&, Timer_t&&, std::chrono::milliseconds) -> Periodic_timer<Callb>;

template <typename Callb, typename Timer_t>
decltype(auto) make_periodic_timer(Callb &&callb, Timer_t &&cmd_timer, std::chrono::milliseconds interval = std::chrono::seconds{60})
{
    return std::make_unique<Periodic_timer<Callb>>(std::forward<Callb>(callb), std::forward<Timer_t>(cmd_timer), interval);
}