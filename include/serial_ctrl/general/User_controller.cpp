#include <User_controller.hpp>

using namespace Logic::Port_settings;

namespace Logic
{
    void User_controller::set_baud_rate(const unsigned int baud_rate)
    {
        delegator.delegate_job(Job{Setting::Baud_rate, baud_rate});
    }

    void User_controller::set_parity(const Port_settings::Parity parity)
    {
        delegator.delegate_job(Job{Setting::Parity, parity});
    }

    void User_controller::set_char_size(const unsigned int char_size)
    {
        delegator.delegate_job(Job{Setting::Char_size, char_size});
    }

    void User_controller::set_stop_bits(const Port_settings::Stop_bits stop_bits)
    {
        delegator.delegate_job(Job{Setting::Stop_bits, stop_bits});
    }
}
