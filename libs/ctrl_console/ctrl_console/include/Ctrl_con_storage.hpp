#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_con_storage : virtual protected Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>
    {
    public:
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle);

        template <typename Cmd_pair_t>
        void add_cmd(Cmd_pair_t &&cmd);
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Str, typename Handle_t>
    void Ctrl_con_storage<Base_handle_t, endl_opt, endl, space>::add_cmd(Str &&cmd_name, Handle_t &&handle)
    {
        if (cmds.find(cmd_name) == cmds.end())
        {
            cmds.emplace(std::forward<Str>(cmd_name),
                         Ctrl_handle(std::make_unique<Handle_t>(std::forward<Handle_t>(handle))));
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name already exists!"};
        }
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    template <typename Cmd_pair_t>
    inline void Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>::add_cmd(Cmd_pair_t &&cmd)
    {
        if (cmds.find(cmd.first) == cmds.end())
        {
            cmds.insert(std::forward<Cmd_pair_t>(cmd));
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name already exists!"};
        }
    }
}