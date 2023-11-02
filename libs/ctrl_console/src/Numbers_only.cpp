#include <policies/Numbers_only.hpp>

namespace Cmd_ctrl
{
    bool Numbers_only::validate(const std::string &arg)
    {
        return true;
    }

    bool Numbers_only::validate_t(const std::string &arg)
    {
        return true;
    }

    bool Numbers_only::validate_t()
    {
        return true;
    }

}