#pragma once

#include <Base_ctrl_console.hpp>
#include <Ip_serial_ctrl.hpp>
#include <Cmds_except.hpp>

using namespace Cmd_ctrl;

namespace Ip_serial
{
    using Handle_t = Cmd_ctrl::Exec<Ip_serial::Ip_serial_ctrl&, const std::string &>::Addons<>;

    class Console : protected Base_ctrl_console<Handle_t::Base_handle_intf, Endl_opt::with>
    {
    public:
        template <typename Iter_t>
        void exec(Ip_serial::Ip_serial_ctrl &info, const Iter_t begin, const Iter_t end) const
        {
            auto parsed_cmds = parser.parse(begin, end);

            std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &info](auto &p_cmd)
                        {
                        if (cmds[p_cmd.name]->validate(p_cmd.arg))
                            cmds[p_cmd.name]->exec(info, std::move(p_cmd.arg));
                        else
                            throw Exception::Cmds_except{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"}; });
        }
    };
}