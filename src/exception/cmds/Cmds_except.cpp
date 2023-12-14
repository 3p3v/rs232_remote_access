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

    Cmds_timeout::Cmds_timeout(const std::string &expln)
        : Exception{"Cmds_timeout", expln}
    {
    }

    Cmds_timeout::Cmds_timeout(const char *expln)
        : Exception{"Cmds_timeout", expln}
    {
    }

    Cmds_bad_cmd::Cmds_bad_cmd(const std::string &expln)
        : Exception{"Cmds_bad_cmd", expln}
    {
    }

    Cmds_bad_cmd::Cmds_bad_cmd(const char *expln)
        : Exception{"Cmds_bad_cmd", expln}
    {
    }

    Cmds_no_packet::Cmds_no_packet(const std::string &expln)
        : Exception{"Cmds_no_packet", expln}
    {
    }

    Cmds_no_packet::Cmds_no_packet(const char *expln)
        : Exception{"Cmds_no_packet", expln}
    {
    }
}