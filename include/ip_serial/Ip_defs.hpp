#pragma once 

#include <string>
#include <boost/bimap.hpp>
#include <Ctrl_interface.hpp>

using namespace Serial_port;

class Ip_defs
{
    using Parity_bimap = boost::bimap<Ctrl_defs::Parity, std::string>;
    using Flow_ctrl_bimap = boost::bimap<Ctrl_defs::Flow_ctrl, std::string>;
    using Stop_bits_bimap = boost::bimap<Ctrl_defs::Stop_bits, std::string>;
    
    static constexpr std::string_view ok_str{"_ok"};

    static const Parity_bimap parity_bi;
    static const Flow_ctrl_bimap flow_ctrl_bi;
    static const Stop_bits_bimap stop_bits_bi;

public:
    static constexpr std::string_view set_baud_rate_s{"set_baud_rate"};
    static constexpr std::string_view set_parity_s{"set_parity"};
    static constexpr std::string_view set_char_size_s{"set_char_size"};
    static constexpr std::string_view set_flow_ctrl_s{"set_flow_ctrl"};
    static constexpr std::string_view set_stop_bits_s{"set_stop_bits"};

    static std::string ok_trans(const std::string_view &opt_name);
    static std::string parity_trans(Serial_port::Ctrl_defs::Parity parity);
    static std::string flow_ctrl_trans(Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl);
    static std::string stop_bits_trans(Serial_port::Ctrl_defs::Stop_bits stop_bits);
    static Serial_port::Ctrl_defs::Parity parity_trans(const std::string &parity);
    static Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl_trans(const std::string &flow_ctrl);
    static Serial_port::Ctrl_defs::Stop_bits stop_bits_trans(const std::string &stop_bits);
};