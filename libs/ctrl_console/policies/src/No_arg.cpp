#include <No_arg.hpp>
#include <algorithm>

bool Cmd_ctrl::No_arg::validate_t(const std::string &arg)
{
    if (arg.size() != 0)
        return false;

    return true;
}