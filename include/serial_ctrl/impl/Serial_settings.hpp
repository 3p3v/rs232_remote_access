#pragma once

#include <stdexcept>
#include <Serial_base.hpp>

using namespace Serial_port;

namespace Impl
{
    class Serial_settings : protected Serial_base
    {
    public:
        void set_baud_rate(unsigned int baud_rate);
        void set_parity(const Parity parity);
        void set_char_size(unsigned int char_size);
        void set_stop_bits(const Stop_bits stop_bits);

        Serial_settings(std::shared_ptr<Serial_helper> &&serial);
        Serial_settings(const std::shared_ptr<Serial_helper> &serial);
    };
}