#include <Serial_impl.hpp>

using namespace Serial_port;
using namespace Logic;

namespace Impl
{
    Serial_impl::Serial_impl(Serial_info &info)
        : info{info}
    {
    }

    unsigned int Serial_impl::baud_rate_trans(Port_settings::Baud_rate baud_rate)
    {
        return baud_rate;
    }

    Ctrl_defs::Parity Serial_impl::parity_trans(Port_settings::Parity parity)
    {
        switch (parity)
        {
        case Port_settings::Parity::none:
            return Ctrl_defs::Parity::none;
            break;

        case Port_settings::Parity::odd:
            return Ctrl_defs::Parity::odd;
            break;

        case Port_settings::Parity::even:
            return Ctrl_defs::Parity::even;
            break;

        default:
            throw "Parity option not supported!";
        }
    }

    unsigned char Serial_impl::char_size_trans(Port_settings::Char_size char_size)
    {
        if (char_size <= std::numeric_limits<unsigned char>::max())
        {
            return char_size;
        }
        else
        {
            throw "Not supported character size!";
        }
    }

    Ctrl_defs::Stop_bits Serial_impl::stop_bits_trans(Port_settings::Stop_bits stop_bits)
    {
        switch (stop_bits)
        {
        case Port_settings::Stop_bits::one:
            return Ctrl_defs::Stop_bits::one;
            break;

        case Port_settings::Stop_bits::onepointfive:
            return Ctrl_defs::Stop_bits::onepointfive;
            break;

        case Port_settings::Stop_bits::two:
            return Ctrl_defs::Stop_bits::two;
            break;

        default:
            throw "Stop bits option not supported!";
        }
    }


    void Serial_impl::set_baud_rate(Port_settings::Baud_rate baud_rate)
    {
        serial->set_baud_rate(baud_rate_trans(baud_rate));
    }

    void Serial_impl::set_parity(Port_settings::Parity parity)
    {
        serial->set_parity(parity_trans(parity));
    }

    void Serial_impl::set_char_size(Port_settings::Char_size char_size)
    {
        serial->set_char_size(char_size_trans(char_size));
    }

    void Serial_impl::set_stop_bits(Port_settings::Stop_bits stop_bits)
    {
        serial->set_stop_bits(stop_bits_trans(stop_bits));
    }
}