#pragma once

#include <Ctrl_interface.hpp>

namespace Ip_serial
{
    class Serial_info
    {
    public:
        unsigned int baud_rate;
        Serial_port::Ctrl_defs::Parity parity;
        unsigned int char_size;
        Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl;
        Serial_port::Ctrl_defs::Stop_bits stop_bits;

        Serial_info() = default;
        Serial_info(Serial_info &&) = default;
        Serial_info &operator=(Serial_info &&) = delete;
        Serial_info(Serial_info &) = default;
        Serial_info &operator=(Serial_info &) = delete;
        virtual ~Serial_info() = 0;
    };
}