#pragma once

#include <algorithm>
#include <string>
#include <Base_ctrl_console.hpp>
#include <Exec.hpp>
#include <addons/Mandatority.hpp>

namespace Cmd_ctrl
{
    namespace Setup
    {
        using Handle_t = Exec<std::string &&>::Addons<Mandatority>;

        class Setup_console final : public Base_ctrl_console<Handle_t::Base_handle_intf, Endl_opt::without>
        {
        public:
            template <typename Iter_t>
            void exec(const typename Iter_t begin, const typename Iter_t end);

            void check();
        };

        template <typename Iter_t>
        void Setup_console::exec(const typename Iter_t begin, const typename Iter_t end)
        {
            auto parsed_cmds = parser.parse(begin, end);

            std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
                          {
                    if (cmds[p_cmd.name]->validate(p_cmd.arg))
                    {
                        cmds[p_cmd.name]->exec(std::move(p_cmd.arg));
                        cmds[p_cmd.name]->done();
                    }
                    else
                    {
                        throw std::runtime_error("Received command: \"" + p_cmd.name + "\" didn't pass validation!"); 
                    } });
        }
    }
}