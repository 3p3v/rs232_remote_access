#include <Cmds_except.hpp>

namespace Exception
{
    Cmds_except::Cmds_except(const std::string &expln)
        : Exception{"CMDS", expln}
    {
    }

    Cmds_except::Cmds_except(const char *expln)
        : Exception{"CMDS", expln}
    {
    }
}