#include <Is_cmd.hpp>
#include <algorithm>
#include <cctype>

namespace Cmd_ctrl
{
    bool Is_cmd::validate_t(const std::string &arg)
    {
        if (std::find_if_not(
                arg.cbegin(),
                arg.cend(),
                [](unsigned char num)
                {
                    return std::isupper(num) || num == '_';
                }) != arg.end())
            return false;

        return true;
    }
}