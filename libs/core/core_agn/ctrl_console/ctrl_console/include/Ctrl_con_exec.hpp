#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_con_exec
    {
    public:
        using Base_ctrl_console_t = Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_>;

    private:
        Base_ctrl_console_t &cc;

    public:
        template <typename Iter_t>
        void exec(const Iter_t begin, const Iter_t end) const;

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