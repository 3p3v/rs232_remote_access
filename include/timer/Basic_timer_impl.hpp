#pragma once

#include <boost/asio.hpp>
#include <Basic_timer.hpp>
#include <serial/Serial_context.hpp>
#include <Cmds_except.hpp>

/// @brief Basic_timer_impl fireing callback after given time
class Basic_timer_impl : public Basic_timer, protected Phy_serial::Serial_context
{
protected:
    /* Basic_timer_impl */
    boost::asio::steady_timer timer{*shared_io_context_};

public:
    void start() = 0;
    void stop() override;

    Basic_timer_impl(std::chrono::milliseconds interval = std::chrono::seconds{10});
    Basic_timer_impl(Basic_timer_impl &&) = default;
    Basic_timer_impl &operator=(Basic_timer_impl &&) = default;
    Basic_timer_impl(const Basic_timer_impl &) = delete;
    Basic_timer_impl &operator=(const Basic_timer_impl &) = delete;
    ~Basic_timer_impl();
};