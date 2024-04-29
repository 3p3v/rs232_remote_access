#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_con_exec : virtual protected Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>
    {
    public:
        template <typename... Args, typename Iter_t>
        void exec(const Iter_t begin, const Iter_t end, Args &&...args) const;
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename... Args, typename Iter_t>
    void Ctrl_con_exec<Base_handle_t, endl_opt, endl, space>::exec(const Iter_t begin, const Iter_t end, Args &&...args) const
    {
        auto parsed_cmds = parser.parse(begin, end);

        std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &args...](auto &p_cmd)
                      {
                        if (cmds[p_cmd.name]->validate(p_cmd.arg))
                            cmds[p_cmd.name]->exec(std::forward<Args>(args) ..., std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
                        else
                            throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"}; });
    }
}