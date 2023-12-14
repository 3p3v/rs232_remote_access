#pragma once

#include <Exception.hpp>

namespace Exception
{
    class Cmds_except : public Exception
    {
    public:
        Cmds_except(const std::string &expln);
        Cmds_except(const char *expln);
    };

    class Cmds_timeout : public Exception
    {
    public:
        Cmds_timeout(const std::string &expln);
        Cmds_timeout(const char *expln);
    };

    class Cmds_bad_cmd : public Exception
    {
    public:
        Cmds_bad_cmd(const std::string &expln);
        Cmds_bad_cmd(const char *expln);
    };

    class Cmds_no_packet : public Exception
    {
    public:
        Cmds_no_packet(const std::string &expln);
        Cmds_no_packet(const char *expln);
    };
}