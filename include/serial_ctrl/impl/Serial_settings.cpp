#include <Serial_settings.hpp>

namespace Impl
{
    void Serial_settings::set_baud_rate(unsigned int baud_rate)
    {
        auto serial = get_serial();
        serial.set_baud_rate(baud_rate);
    }

    void Serial_settings::set_parity(const Parity parity)
    {
        auto serial = get_serial();
        serial.set_parity(parity);
    }

    void Serial_settings::set_char_size(unsigned int char_size)
    {
        auto serial = get_serial();
        serial.set_char_size(char_size);
    }

    void Serial_settings::set_stop_bits(const Stop_bits stop_bits)
    {
        auto serial = get_serial();
        serial.set_stop_bits(stop_bits);
    }

    Serial_settings::Serial_settings(std::shared_ptr<Serial_helper> &&serial)
        : Serial_base{std::move(serial)}
    {
    }

    Serial_settings::Serial_settings(const std::shared_ptr<Serial_helper> &serial)
        : Serial_settings{serial}
    {
    }
}