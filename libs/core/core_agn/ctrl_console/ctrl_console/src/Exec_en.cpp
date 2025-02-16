#include <Exec_en.hpp>

void Cmd_ctrl::Exec_en::enable(bool enable)
{
    if (!blocked)
    {
        enabled = enable;
    }
}

void Cmd_ctrl::Exec_en::enable()
{
    if (!blocked)
    {
        enabled = true;
    }
}

void Cmd_ctrl::Exec_en::disable()
{
    if (!blocked)
    {
        enabled = false;
    }
}

inline bool Cmd_ctrl::Exec_en::validate(const std::string &arg) const noexcept
{
    return enabled;
}

inline Cmd_ctrl::Exec_en::Exec_en(bool enabled, bool blocked)
    : enabled{enabled}, blocked{blocked}
{
}
