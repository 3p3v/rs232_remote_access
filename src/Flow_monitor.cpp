#include <User_channel_monitor.hpp>

size_t Flow_monitor::get_out(const std::string& dev_name)
{
    return serials[dev_name].second;
}

size_t Flow_monitor::get_in(const std::string& dev_name)
{
    return serials[dev_name].first;
}

void Flow_monitor::add_in(const std::string& dev_name, size_t rec_len)
{
    serials[dev_name].first += rec_len;
}

void Flow_monitor::add_out(const std::string& dev_name, size_t write_len)
{
    serials[dev_name].second += write_len;
}

