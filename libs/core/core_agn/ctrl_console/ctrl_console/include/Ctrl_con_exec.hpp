#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    /// @brief Interface for executing commands
    class Ctrl_con_exec
    {
        Base_ctrl_console &cc;

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

        Ctrl_con_exec(Base_ctrl_console &cc);
    };

    template <typename Iter_t>
    void Ctrl_con_exec::exec(const Iter_t begin, const Iter_t end) const
    {
        auto parsed_cmds = cc.parser.parse(begin, end);

        std::for_each(
            parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
            {
                auto itr = cc.cmds.find(p_cmd.name);
                
                if (itr != cc.cmds.end())
                {
                    if (itr->second.enabled) 
                    {
                        if (itr->second.exec->validate(p_cmd.arg))
                        {
                            itr->second.exec->exec(std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
                        }
                        else
                        {
                            throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};
                        }
                    }
                    else
                    {
                        // TODO
                    }
                }
                else
                {
                    throw std::logic_error{"Command \"" + p_cmd.name + "\" not found!"};
                } });
    }

    template <typename Iter_t, typename Callb_t>
    void Ctrl_con_exec::exec(const Iter_t begin, const Iter_t end, const Callb_t &callb) const
    {
        auto parsed_cmds = cc.parser.parse(begin, end);

        std::for_each(
            parsed_cmds.begin(), parsed_cmds.end(), [this, &callb](auto &p_cmd)
            {
            auto itr = cc.cmds.find(p_cmd.name);
            
            if (itr != cc.cmds.end())
            {
                if (itr->second.enabled) 
                {
                    if (itr->second.exec->validate(p_cmd.arg))
                    {
                        itr->second.exec->exec(std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
                        callb(p_cmd.name);
                    }
                    else
                    {
                        throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};
                    }
                }
                else
                {
                    // TODO
                }
            }
            else
            {
                throw std::logic_error{"Command \"" + p_cmd.name + "\" not found!y"};
            } });
    }
}