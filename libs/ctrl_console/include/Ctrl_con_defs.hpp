#pragma once

namespace Cmd_ctrl
{
    class Ctrl_con_defs
    {
    public:
        static constexpr char endl = '\n';
        static constexpr char space = ' ';

        Ctrl_con_defs() = default;
        Ctrl_con_defs(Ctrl_con_defs&&) = default;
        Ctrl_con_defs& operator=(Ctrl_con_defs&&) = default;
        Ctrl_con_defs(Ctrl_con_defs&) = default;
        Ctrl_con_defs& operator=(Ctrl_con_defs&) = default;
        virtual ~Ctrl_con_defs() = 0;
    };
}