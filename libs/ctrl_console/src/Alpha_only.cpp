#include <Alpha_only.hpp>
#include <algorithm>
#include <cctype>

bool Cmd_ctrl::Alpha_only::validate_t(const std::string &arg)
{
    if (std::find_if_not(
            arg.cbegin(),
            arg.cend(),
            [](unsigned char num)
            {
                return std::isalpha(num);
            }
        ) != arg.end())
            return false;

        return true;
}