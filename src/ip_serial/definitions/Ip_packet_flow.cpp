#include <Ip_packet_flow.hpp>

namespace Ip_serial
{
    char Ip_packet_flow::num_up_()
    {
        char temp = next_num;

        /* Increment not acked */
        not_acked++;

        /*  */
        if (++next_num > max_msg_num)
        {
            next_num = min_msg_num;
        }

        return temp;
    }

    unsigned char Ip_packet_flow::get_not_acked()
    {
        return not_acked;
    }

    void Ip_packet_flow::ack(char id)
    {
        not_acked -= next_num - id - 1;
    }

    char Ip_packet_flow::exp()
    {
        return next_num;
    }

    Ip_packet_flow::~Ip_packet_flow() = default;
}