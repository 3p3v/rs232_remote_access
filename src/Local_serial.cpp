#include <Local_serial.hpp>

using namespace Serial_port;

/* Callbacks */
void Local_serial::write_callback(std::size_t write_len)
{
    monitor.get()->add_out(write_len);
}

void Local_serial::read_callback(const Data &data, std::size_t write_len)
{
}

void Local_serial::error_callback(const unsigned int code, const std::string &err)
{
    throw std::runtime_error("Exception [" + err + "] occured on serial port [" + get_port() + "] while writing.");
}
