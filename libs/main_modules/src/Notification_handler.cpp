#include <Notification_handler.hpp>
#include <iostream>

Logic::Notification_handler::Notification_handler( // TODO
    bool close_on_timeout,
    bool close_on_data_loss, 
    bool close_on_protocol_error, 
    bool debug)
    : debug{debug}
{
}

void Logic::Notification_handler::error(Dev_num dev_num, const std::exception & exc) const
{
}

void Logic::Notification_handler::debug(Dev_num dev_num, const std::string &info) const
{
    if (debug)
    {
        std::clog << "[Device: " << dev_num << "]\n"; // TODO insert device name
        std::cout << info << '\n\n';
    }
}