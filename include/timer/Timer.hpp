#pragma once 

#include <boost/asio.hpp>
#include <Basic_timer.hpp>
#include <serial/Serial_context.hpp>
#include <Cmds_except.hpp>
#include <Monitor.hpp>

/// @brief Timer fireing callback after given time
class Timer : public Basic_timer, public Phy_serial::Serial_context
{
    /* Command name */
    std::string cmd_name;

    /* Timer */
    boost::asio::steady_timer timer{*shared_io_context_};

    Monitor &monitor;

public:
    void start() override;
    void stop() override;

    template <typename Str>
    Timer(Str &&cmd_name, Monitor &monitor);
    Timer(Timer&&) = default;
    Timer& operator=(Timer&&) = default;
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    ~Timer();
};

template <typename Str>
Timer::Timer(Str &&cmd_name, Monitor &monitor)
    : Basic_timer{std::chrono::seconds{10}}, cmd_name{std::forward<Str>(cmd_name)}, monitor{monitor}
{
}