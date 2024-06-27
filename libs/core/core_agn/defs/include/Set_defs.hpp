#pragma once 

#include <string>
#include <array>
#include <boost/bimap.hpp>
#include <Port_settings.hpp>

// using namespace Serial_port;

namespace Logic
{
    class Set_defs
    {
        using Parity_bimap = boost::bimap<Port_settings::Parity, std::string>;
        // using Flow_ctrl_bimap = boost::bimap<Ctrl_defs::Flow_ctrl, std::string>;
        using Stop_bits_bimap = boost::bimap<Port_settings::Stop_bits, std::string>;
        
        // static constexpr std::string_view ok_str{"_ok"};

        static const Parity_bimap parity_bi;
        // static const Flow_ctrl_bimap flow_ctrl_bi;
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
        static std::string baud_rate_trans(Port_settings::Baud_rate baud_rate);
        static std::string parity_trans(Port_settings::Parity parity) noexcept;
        static std::string char_size_trans(Port_settings::Char_size char_size);
        // static std::string flow_ctrl_trans(Port_settings::Flow_ctrl flow_ctrl);
        static std::string stop_bits_trans(Port_settings::Stop_bits stop_bits) noexcept;
        static Port_settings::Baud_rate baud_rate_trans(const std::string &baud_rate);
        static Port_settings::Parity parity_trans(const std::string &parity);
        static Port_settings::Char_size char_size_trans(const std::string &baud_rate);
        // static Port_settings::Flow_ctrl flow_ctrl_trans(const std::string &flow_ctrl);
        static Port_settings::Stop_bits stop_bits_trans(const std::string &stop_bits);

        Set_defs() = delete;
    };
}