#pragma once 

#include <string>
#include <boost/bimap.hpp>
#include <Ctrl_interface.hpp>

using namespace Serial_port;

namespace Ip_serial
{
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

        Ip_defs() = default;
        Ip_defs(Ip_defs&&) = default;
        Ip_defs& operator=(Ip_defs&&) = default;
        Ip_defs(const Ip_defs&) = default;
        Ip_defs& operator=(const Ip_defs&) = default;
        virtual ~Ip_defs() = 0;
    };

    class Ip_hi
    {
    public:
        static constexpr std::string_view master_hi_s{"M_HI"};
        static constexpr std::string_view slave_hi_s{"S_HI"};
        static constexpr std::string_view master_keep_alive_s{"MK_A"};
        static constexpr std::string_view slave_keep_alive_s{"SK_A"};
        
        static constexpr std::string_view get_info_s{"G_I"};

        static constexpr std::string_view get_baud_rate_s{"get_baud_rate"};
        static constexpr std::string_view get_parity_s{"get_parity"};
        static constexpr std::string_view get_char_size_s{"get_char_size"};
        static constexpr std::string_view get_flow_ctrl_s{"get_flow_ctrl"};
        static constexpr std::string_view get_stop_bits_s{"get_stop_bits"};

        Ip_hi() = default;
        Ip_hi(Ip_hi&&) = default;
        Ip_hi& operator=(Ip_hi&&) = default;
        Ip_hi(const Ip_hi&) = default;
        Ip_hi& operator=(const Ip_hi&) = default;
        virtual ~Ip_hi() = 0;
    };
}