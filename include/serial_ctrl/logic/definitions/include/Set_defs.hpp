#pragma once 

#include <string>
#include <array>
#include <boost/bimap.hpp>
#include <Ctrl_interface.hpp>

using namespace Serial_port;

namespace Ip_serial
{
    class Set_defs
    {
        using Parity_bimap = boost::bimap<Ctrl_defs::Parity, std::string>;
        using Flow_ctrl_bimap = boost::bimap<Ctrl_defs::Flow_ctrl, std::string>;
        using Stop_bits_bimap = boost::bimap<Ctrl_defs::Stop_bits, std::string>;
        
        // static constexpr std::string_view ok_str{"_ok"};

        static const Parity_bimap parity_bi;
        static const Flow_ctrl_bimap flow_ctrl_bi;
        static const Stop_bits_bimap stop_bits_bi;

    public:
        static constexpr std::string_view set_baud_rate_s{"S_B"};
        static constexpr std::string_view set_parity_s{"S_P"};
        static constexpr std::string_view set_char_size_s{"S_C"};
        static constexpr std::string_view set_stop_bits_s{"S_S"};

        static constexpr std::string_view cts_set_s{"C_S"};
        static constexpr std::string_view cts_reset_s{"C_R"};
        static constexpr std::string_view rts_set_s{"R_S"};
        static constexpr std::string_view rts_reset_s{"R_R"};

        // static std::string ok_trans(const std::string_view &opt_name);
        static std::string baud_rate_trans(unsigned int baud_rate);
        static std::string parity_trans(Serial_port::Ctrl_defs::Parity parity);
        static std::string char_size_trans(unsigned int baud_rate);
        static std::string flow_ctrl_trans(Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl);
        static std::string stop_bits_trans(Serial_port::Ctrl_defs::Stop_bits stop_bits);
        static unsigned int baud_rate_trans(const std::string &baud_rate);
        static Serial_port::Ctrl_defs::Parity parity_trans(const std::string &parity);
        static unsigned int char_size_trans(const std::string &baud_rate);
        static Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl_trans(const std::string &flow_ctrl);
        static Serial_port::Ctrl_defs::Stop_bits stop_bits_trans(const std::string &stop_bits);

        Set_defs() = default;
        Set_defs(Set_defs&&) = default;
        Set_defs& operator=(Set_defs&&) = default;
        Set_defs(const Set_defs&) = default;
        Set_defs& operator=(const Set_defs&) = default;
        virtual ~Set_defs() = 0;
    };
}