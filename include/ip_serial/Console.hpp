#pragma once

#include <Base_ctrl_console.hpp>
#include <Serial_ctrl_helper.hpp>

using namespace Cmd_ctrl;

namespace Ip_serial
{
    using Handle_t = Cmd_ctrl::Exec<Serial_ctrl_helper&, const std::string &>::Addons<>;

    class Console : protected Base_ctrl_console<Handle_t::Base_handle_intf, Endl_opt::with>
    {
    public:
        template <typename Iter_t>
        void exec(Serial_ctrl_helper& info, const Iter_t begin, const Iter_t end) const
        {
            auto parsed_cmds = parser.parse(begin, end);

            std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &info](auto &p_cmd)
                        {
                        if (cmds[p_cmd.name]->validate(p_cmd.arg))
                            cmds[p_cmd.name]->exec(info, std::move(p_cmd.arg));
                        else
                            throw std::runtime_error("Received command: \"" + p_cmd.name + "\" didn't pass validation!"); });
        }
    };
}