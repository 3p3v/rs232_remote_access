#include <Ip_slave.hpp>
#include <stdexcept>

namespace Ip_serial
{
    char Ip_slave::num_up(char num)
    {
        if (s_reload)
        {
            s_reload = false;
            next_num = num + 1;
            return next_num;
        }
        else
        {
            if (next_num != num)
                throw std::logic_error{""};

            return Ip_packet_flow::num_up_();
        }
    }

    void Ip_slave::reload()
    {
        s_reload = true;
    }
}
