#include <Packet_sett_final.hpp>

using namespace Logic;

const Packet_sett_final &Packet_sett_final::get()
{
    static const Packet_sett_final p{Packet_defs::min_msg_num, Packet_defs::max_msg_num, Packet_defs::max_saved};

    return p;
}
