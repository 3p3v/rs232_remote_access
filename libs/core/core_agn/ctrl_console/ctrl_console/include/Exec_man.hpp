#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Exec_man
    {
    public:
        enum class Mandatoriness : bool
        {
            Mandatory = true,
            Optional = false
        };

    private:
        const Mandatoriness mandatoriness;
        bool executed{false};

    public:
        bool validate(const std::string &arg) const noexcept;

        bool check_executed();

        Exec_man(Mandatoriness mandatoriness = Mandatoriness::Mandatory);
    };
} // namespace Cmd_ctrl
