#include <Upper_only.hpp>
#include <algorithm>
#include <cctype>

bool Cmd_ctrl::Upper_only::validate_t(const std::string &arg)
{
    if (std::find_if_not(
            arg.cbegin(),
            arg.cend(),
            [](unsigned char num)
            {
                return std::isupper(num);
            }) != arg.end())
        return false;

    return true;
}