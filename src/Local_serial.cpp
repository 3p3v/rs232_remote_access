#include <Local_serial.hpp>

using namespace Serial_port;

/* Callbacks */
void Local_serial::write_callback(std::size_t write_len)
{
    monitor.get()->add_out(write_len);
}

void Local_serial::read_callback(Cont_type::iterator begin, Cont_type::iterator end, std::size_t write_len)
{
}

void Local_serial::error_callback(const std::string &err)
{
    throw std::runtime_error("Exception [" + err + "] occured on serial port [" + get_port() + "] while writing.");
}
