#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    /// @brief Interface for executing commands
    /// @tparam Base_handle_t Specyfication of Exec<... Args_t>::Param<Args>::Policies<... Policies_t>::Base_handle // TODO delete, this is unused and unnessesary
    /// @tparam endl_opt Checking endline enabled/disabled
    /// @tparam endl_ Character between different commands
    /// @tparam space_ Character between parameter name and argument 
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_con_exec
    {
    public:
        using Base_ctrl_console_t = Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>;

    private:
        Base_ctrl_console_t &cc;

    public:
        /// @brief Find and execute commands
        /// @tparam Iter_t Iterator
        /// @param begin std::begin of data
        /// @param end std::end of data
        template <typename Iter_t>
        void exec(const Iter_t begin, const Iter_t end) const;

        /// @brief Find and execute commands
        /// @tparam Iter_t Iterator
        /// @tparam Callb_t Functor, same type as command handler
        /// @param begin std::begin of data
        /// @param end std::end of data
        /// @param callb Callback to be fired after execution of each command
        template <typename Iter_t, typename Callb_t>
        void exec(const Iter_t begin, const Iter_t end, const Callb_t &callb) const;

        Ctrl_con_exec(Base_ctrl_console_t &cc);
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Iter_t>
    void Ctrl_con_exec<Base_handle_t, endl_opt, endl, space>::exec(const Iter_t begin, const Iter_t end) const
    {
        auto parsed_cmds = cc.parser.parse(begin, end);

        std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
                      {
                        auto itr = cc.cmds.find(p_cmd.name);
                        
                        if (itr != cc.cmds.end())
                        {
                            if (itr->second->validate(p_cmd.arg))
                            {
                                itr->second->exec(std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
                            }
                            else
                            {
                                throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};
                            }
                        }
                        else
                        {
                            throw std::logic_error{"Command \"" + p_cmd.name + "\" not found!y"};
                        } });
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Iter_t, typename Callb_t>
    void Ctrl_con_exec<Base_handle_t, endl_opt, endl, space>::exec(const Iter_t begin, const Iter_t end, const Callb_t &callb) const
    {
        auto parsed_cmds = cc.parser.parse(begin, end);

        std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &callb](auto &p_cmd)
                      {
                        auto itr = cc.cmds.find(p_cmd.name);
                        
                        if (itr != cc.cmds.end())
                        {
                            if (itr->second->validate(p_cmd.arg))
                            {
                                itr->second->exec(std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
                                callb(p_cmd.name);
                            }
                            else
                            {
                                throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};
                            }
                        }
                        else
                        {
                            throw std::logic_error{"Command \"" + p_cmd.name + "\" not found!y"};
                        } });
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    Ctrl_con_exec<Base_handle_t, endl_opt, endl, space>::Ctrl_con_exec(Base_ctrl_console_t &cc)
        : cc{cc}
    {
    }
}