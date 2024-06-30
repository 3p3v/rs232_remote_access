#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    /// @brief Interface for adding commands
    /// @tparam Base_handle_t Specyfication of Exec<... Args_t>::Param<Args>::Policies<... Policies_t>::Base_handle // TODO delete, this is unused and unnessesary
    /// @tparam endl_opt Checking endline enabled/disabled
    /// @tparam endl_ Character between different commands
    /// @tparam space_ Character between parameter name and argument 
    template <
        typename Base_handle_t, 
        Endl_opt endl_opt, 
        char endl_ = '\n', 
        char space_ = ' '>
    class Ctrl_con_storage
    {
        using Base_ctrl_console_t= Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>;
        using Ctrl_handle = typename Base_ctrl_console_t::Ctrl_handle;

        Base_ctrl_console_t &cc;

    public:
        /// @brief Add new command
        /// @tparam Str std::string or any other type that string can be constructed from
        /// @tparam Handle_t Function type void(Base_handle_t::Arg)
        /// @param cmd_name Command name
        /// @param handle Handler to be executed of type void(Base_handle_t::Arg)
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle);

        /// @brief Add new command
        /// @tparam Cmd_pair_t std::pair
        /// @param cmd Command name & handler to be executed of type void(Base_handle_t::Arg)
        template <typename Cmd_pair_t>
        void add_cmd(Cmd_pair_t &&cmd);

        /// @brief Remove cmd with given name
        /// @tparam Str
        /// @param cmd_name
        template <typename Str>
        void remove_cmd(const Str &cmd_name);

        /// @brief Remove many cmds with given names
        /// @tparam Iter_t Iterator
        /// @param begin First command to be erased
        /// @param end Last + 1 command to be erased
        template <typename Iter_t>
        void remove_cmd(Iter_t begin, Iter_t end);

        Ctrl_con_storage(Base_ctrl_console_t &cc);
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Str, typename Handle_t>
    void Ctrl_con_storage<Base_handle_t, endl_opt, endl, space>::add_cmd(Str &&cmd_name, Handle_t &&handle)
    {
        if (cc.cmds.find(cmd_name) == cc.cmds.end())
        {
            cc.cmds.emplace(std::forward<Str>(cmd_name),
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
        if (cc.cmds.find(cmd.first) == cc.cmds.end())
        {
            cc.cmds.insert(std::forward<Cmd_pair_t>(cmd));
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name already exists!"};
        }
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    template <typename Str>
    inline void Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>::remove_cmd(const Str &cmd_name)
    {
        auto cmd_idx = cc.cmds.find(cmd_name);

        if (cmd_idx != cc.cmds.end())
        {
            cc.cmds.erase(cmd_idx);
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name does not exists!"};
        }
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    template <typename Iter_t>
    inline void Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>::remove_cmd(Iter_t begin, Iter_t end)
    {
        std::for_each(
            begin,
            end,
            [this](const auto &cmd_name)
            {
                auto cmd_idx = cc.cmds.find(cmd_name);

                if (cmd_idx != cc.cmds.end())
                {
                    cc.cmds.erase(cmd_idx);
                }
                else
                {
                    throw std::logic_error{"The comand with specyfied name does not exists!"};
                }
            });
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    Ctrl_con_storage<Base_handle_t, endl_opt, endl, space>::Ctrl_con_storage(Base_ctrl_console_t &cc)
        : cc{cc}
    {
    }
}