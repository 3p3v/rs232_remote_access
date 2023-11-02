#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <Base_ctrl_console.hpp>
#include <Exec.hpp>

namespace Cmd_ctrl
{
    namespace Ctrl
    {
        using Handle_t = Exec<const std::string &, const std::string &>::Addons<>;

        class Ctrl_console : public Base_ctrl_console<Handle_t::Base_handle_intf, Endl_opt::with>
        {
        protected:
            virtual void leh(const std::string &dev_name, std::string &&data) = 0;

        public:
            template <typename Iter_t>
            void exec(const std::string &device_name, const typename Iter_t begin, const typename Iter_t end)
            {
                auto parsed_cmds = parser.parse(begin, end);

                std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &device_name](auto &p_cmd)
                              {
                    if (cmds[p_cmd.name]->validate(p_cmd.arg))
                        cmds[p_cmd.name]->exec(device_name, std::move(p_cmd.arg));
                    else
                        throw std::runtime_error("Received command: \"" + p_cmd.name + "\" didn't pass validation!"); });
            }

            void local_exec(const std::string &dev_name, const std::string &name, const std::string &arg)
            {
                if (cmds[name]->validate(arg))
                {
                    leh(dev_name, name + space + arg + endl);
                }
            }
        };
    }
}