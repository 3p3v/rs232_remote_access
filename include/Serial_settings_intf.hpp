#pragma once

#include <string>
#include <Ctrl_interface.hpp>

class Serial_settings_intf
{
public:
    virtual void set_baud_rate(const std::string &dev_name, const unsigned int baud_rate) = 0;
    virtual void set_parity(const std::string &dev_name, const Serial_port::Ctrl_defs::Parity parity) = 0;
    virtual void set_char_size(const std::string &dev_name, const unsigned int char_size) = 0;
    virtual void set_flow_ctrl(const std::string &dev_name, const Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl) = 0;
    virtual void set_stop_bits(const std::string &dev_name, const Serial_port::Ctrl_defs::Stop_bits stop_bits) = 0;
    // virtual void close(const std::string &dev_name) = 0;

    Serial_settings_intf() = default;
    Serial_settings_intf(Serial_settings_intf&&) = default;
    Serial_settings_intf& operator=(Serial_settings_intf&&) = default;
    Serial_settings_intf(Serial_settings_intf&) = default;
    Serial_settings_intf& operator=(Serial_settings_intf&) = default;
    virtual ~Serial_settings_intf() = default;
    
};