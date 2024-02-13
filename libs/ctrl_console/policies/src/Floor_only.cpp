#include <Floor_only.hpp>
#include <algorithm>
#include <cctype>

bool Cmd_ctrl::Floor_only::validate_t(const std::string &arg)
{
    if (std::find_if_not(
            arg.cbegin(),
            arg.cend(),
            [](char num)
            {
                return num == '_';
            }) != arg.end())
        return false;

    return true;
}