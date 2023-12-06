#include <Num_except.hpp>

namespace Exception
{
    Num_except::Num_except(const std::string &expln)
        : Exception{"PACKET_NUMERATION", expln}
    {
    }

    Num_except::Num_except(const char *expln)
        : Exception{"PACKET_NUMERATION", expln}
    {
    }
}