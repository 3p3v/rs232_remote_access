#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Exec_en
    {
        bool blocked;
        bool enabled;

    public:
        void enable(bool enable);
        void enable();
        void disable();

        bool validate(const std::string &arg) const noexcept;

        Exec_en(bool enabled = true, bool blocked = false);
    };
} // namespace Cmd_ctrl
