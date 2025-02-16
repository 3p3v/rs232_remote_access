#include <Exec_man.hpp>

bool Cmd_ctrl::Exec_man::validate(const std::string &arg) const noexcept
{
    executed = true;
    return true;
}

bool Cmd_ctrl::Exec_man::check_executed()
{
    if (executed)
    {
        return true;
    }

    return false;
}

Cmd_ctrl::Exec_man::Exec_man(Mandatoriness mandatoriness)
    : mandatoriness{mandatoriness}
{
}