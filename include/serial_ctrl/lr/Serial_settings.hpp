#pragma once

#include <stdexcept>
#include <Serial.hpp>

using namespace Serial_port;

class Serial_settings
{
    std::unique_ptr<Serial_port::Serial> &serial;

    serial& get_serial();

public:
    void set_baud_rate(unsigned int baud_rate);
    void set_parity(const Parity parity);
    void set_char_size(unsigned int char_size);
    void set_stop_bits(const Stop_bits stop_bits);

    Serial_settings(std::unique_ptr<Serial_port::Serial> &serial);
};
