#pragma once

#include <Ctrl_interface.hpp>
#include <Exec.hpp>

class Serial_info
{
public:
    std::string com;
    unsigned int baud_rate{9600};
    Serial_port::Ctrl_defs::Parity parity{Serial_port::Ctrl_defs::Parity::none};
    unsigned int char_size{8};
    Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl{Serial_port::Ctrl_defs::Flow_ctrl::none};
    Serial_port::Ctrl_defs::Stop_bits stop_bits{Serial_port::Ctrl_defs::Stop_bits::one};
};
