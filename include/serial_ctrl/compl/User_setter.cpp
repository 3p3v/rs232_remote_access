#include <User_setter.hpp>

void User_setter::set_baud_rate(const unsigned int baud_rate)
{
    write_i(
        set_baud_rate_s.data(),
        baud_rate_trans(baud_rate),
        [ptr = shared_from_this_(), this]()
        {
            set_baud_rate_();
        });
}

void User_setter::set_parity(const Serial_port::Ctrl_defs::Parity parity)
{
    write_i(
        set_baud_rate_s.data(),
        parity_trans(parity),
        [ptr = shared_from_this_(), this]()
        {
            set_parity_();
        });
}

void User_setter::set_char_size(const unsigned int char_size)
{
    write_i(
        set_baud_rate_s.data(),
        char_size_trans(char_size),
        [ptr = shared_from_this_(), this]()
        {
            set_char_size_();
        });
}

void User_setter::set_stop_bits(const Serial_port::Ctrl_defs::Stop_bits stop_bits)
{
    write_i(
        set_baud_rate_s.data(),
        stop_bits_trans(stop_bits),
        [ptr = shared_from_this_(), this]()
        {
            set_stop_bits_();
        });
}