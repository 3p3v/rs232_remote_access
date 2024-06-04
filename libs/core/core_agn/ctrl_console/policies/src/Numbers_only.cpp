#include <Numbers_only.hpp>
#include <algorithm>
#include <cctype>

namespace Cmd_ctrl
{
    bool Numbers_only::validate_t(const std::string &arg)
    {
        if (std::find_if_not(
                arg.cbegin(),
                arg.cend(),
                [](unsigned char num)
                {
                    return std::isdigit(num);
                }) != arg.end())
            return false;

        return true;
    }
}