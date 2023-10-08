#include <User_channel_monitor.hpp>

size_t User_channel_monitor::get_out()
{
    return data_out;
}

size_t User_channel_monitor::get_in()
{
    return data_in;
}

void User_channel_monitor::add_in(size_t rec_len)
{
    data_in += rec_len;
}

void User_channel_monitor::add_out(size_t write_len)
{
    data_out += write_len;
}

