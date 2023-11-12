#include <Timer.hpp>
#include <Cmds_except.hpp>
#include <Dispacher.hpp>
#include <Monitor.hpp>
#include <Serial_except.hpp>

void Timer::start() 
{
    timer.cancel();
    timer.expires_after(interval);
    timer.async_wait([this](const boost::system::error_code& ec)
                     {
                        if (!ec)
                            Monitor::get().error(Exception::Cmds_except{"Command: " + cmd_name + " timed out!"});
                        else if (ec != boost::asio::error::operation_aborted)
                            Monitor::get().error(Exception::Serial_except{ec.what()});
                     });
}