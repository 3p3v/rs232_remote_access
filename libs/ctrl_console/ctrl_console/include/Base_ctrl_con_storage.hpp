#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Base_ctrl_con_storage : virtual protected Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>
    {
    public:
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle);

        template <
            template <class, class> typename Cmd_pair_t,
            typename Str_t,
            typename Cmd_ptr_t>
        void add_cmd(Cmd_pair_t<Str_t, Cmd_ptr_t> &&cmd);
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Str, typename Handle_t>
    void Base_ctrl_con_storage<Base_handle_t, endl_opt, endl, space>::add_cmd(Str &&cmd_name, Handle_t &&handle)
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
    template <
        template <class, class> typename Cmd_pair_t,
        typename Str_t,
        typename Cmd_ptr_t>
    inline void Base_ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>::add_cmd(Cmd_pair_t<Str_t, Cmd_ptr_t> &&cmd)
    {
        if (cmds.find(cmd.first) == cmds.end())
        {
            cmds.emplace(std::forward<Str_t>(cmd.first),
                         std::forward<Cmd_ptr_t>(cmd.second));
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name already exists!"};
        }
    }
}