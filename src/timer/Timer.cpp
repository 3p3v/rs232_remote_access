#include <Timer.hpp>

void Timer::start() 
{
    timer.cancel();
    timer.expires_after(interval);
    timer.async_wait([this](const boost::system::error_code& ec)
                     {
                        monitor.error(Exception::Cmds_except{"Command: " + cmd_name + " timed out!"});
                     });
}


void Timer::stop() 
{
    timer.cancel();
}

Timer::~Timer()
{
    stop();
}