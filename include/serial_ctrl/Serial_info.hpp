#pragma once

#include <Ctrl_interface.hpp>
#include <Exec.hpp>

class Serial_info
{
public:
    unsigned int baud_rate;
    Serial_port::Ctrl_defs::Parity parity;
    unsigned int char_size;
    Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl;
    Serial_port::Ctrl_defs::Stop_bits stop_bits;
};
