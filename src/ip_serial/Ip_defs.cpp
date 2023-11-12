#include <Ip_defs.hpp>
#include <boost/assign.hpp>

using namespace Serial_port;

namespace Ip_serial
{
    const Ip_defs::Parity_bimap Ip_defs::parity_bi = boost::assign::list_of<Ip_defs::Parity_bimap::relation>
    (Ctrl_defs::Parity::even, "e")
    (Ctrl_defs::Parity::none, "n")
    (Ctrl_defs::Parity::odd, "o");

    const Ip_defs::Flow_ctrl_bimap Ip_defs::flow_ctrl_bi = boost::assign::list_of<Ip_defs::Flow_ctrl_bimap::relation>
    (Ctrl_defs::Flow_ctrl::hardware, "h")
    (Ctrl_defs::Flow_ctrl::none, "n")
    (Ctrl_defs::Flow_ctrl::software, "s");

    const Ip_defs::Stop_bits_bimap Ip_defs::stop_bits_bi = boost::assign::list_of<Ip_defs::Stop_bits_bimap::relation>
    (Ctrl_defs::Stop_bits::one, "o")
    (Ctrl_defs::Stop_bits::onepointfive, "p")
    (Ctrl_defs::Stop_bits::two, "t");

    std::string Ip_defs::ok_trans(const std::string_view &opt_name)
    {
        return std::string{opt_name} + std::string{ok_str};
    }

    std::string Ip_defs::baud_rate_trans(unsigned int baud_rate)
    {
        return std::to_string(baud_rate);
    }

    std::string Ip_defs::parity_trans(Serial_port::Ctrl_defs::Parity parity)
    {
        return parity_bi.left.find(parity)->get_right();
    }

    std::string Ip_defs::char_size_trans(unsigned int char_size)
    {
        return std::to_string(char_size);
    }

    std::string Ip_defs::flow_ctrl_trans(Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl)
    {
        return flow_ctrl_bi.left.find(flow_ctrl)->get_right();
    }

    std::string Ip_defs::stop_bits_trans(Serial_port::Ctrl_defs::Stop_bits stop_bits)
    {
        return stop_bits_bi.left.find(stop_bits)->get_right();
    }

    unsigned int Ip_defs::baud_rate_trans(const std::string &baud_rate)
    {
        return std::strtoul(baud_rate.data(), nullptr, 0);
    }

    Serial_port::Ctrl_defs::Parity Ip_defs::parity_trans(const std::string &parity)
    {
        return parity_bi.right.find(parity)->get_left();
    }

    unsigned int Ip_defs::char_size_trans(const std::string &char_size)
    {
        return std::strtoul(char_size.data(), nullptr, 0);
    }

    Serial_port::Ctrl_defs::Flow_ctrl Ip_defs::flow_ctrl_trans(const std::string &flow_ctrl)
    {
        return flow_ctrl_bi.right.find(flow_ctrl)->get_left();
    }

    Serial_port::Ctrl_defs::Stop_bits Ip_defs::stop_bits_trans(const std::string &stop_bits)
    {
        return stop_bits_bi.right.find(stop_bits)->get_left();
    }

    Ip_defs::~Ip_defs() = default;
    Ip_hi::~Ip_hi() = default;
}