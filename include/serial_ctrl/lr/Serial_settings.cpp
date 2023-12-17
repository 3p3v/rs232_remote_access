#include <Serial_settings.hpp>

serial &Serial_settings::get_serial()
{
    if (serial != nullptr)
    {
        return serial->get();
    }
    else
    {
        throw std::logic_error{"Serial not initialized, action can't be executed!"};
    }
}

void Serial_settings::set_baud_rate(unsigned int baud_rate)
{
    auto serial =  get_serial();
    serial.set_baud_rate(baud_rate);
}

void Serial_settings::set_parity(const Parity parity)
{
    auto serial =  get_serial();
    serial.set_parity(parity);
}

void Serial_settings::set_char_size(unsigned int char_size)
{
    auto serial =  get_serial();
    serial.set_char_size(char_size);
}

void Serial_settings::set_stop_bits(const Stop_bits stop_bits)
{
    auto serial =  get_serial();
    serial.set_stop_bits(stop_bits);
}

Serial_settings::Serial_settings(std::unique_ptr<Serial_port::Serial> &serial)
    : serial{serial}
{
}