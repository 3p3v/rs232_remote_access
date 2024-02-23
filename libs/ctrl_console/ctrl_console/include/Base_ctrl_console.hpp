#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <Ctrl_con_defs.hpp>
// #include <Common_defs.hpp>
#include <Ctrl_parser.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Base_ctrl_console// : protected Common_defs //, protected Ctrl_con_defs
    {
    public:
        using Handle = Base_handle_t;

    protected:
        using Ctrl_cmd_name = std::string;
        using Ctrl_handle = std::unique_ptr<typename Base_handle_t::Base_handle_intf>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_name,
                                             Ctrl_handle>;

        static constexpr char endl = endl_;
        static constexpr char space = space_;

        mutable Cmds_cont cmds;
        Ctrl_parser<endl_opt, endl, space> parser;

    public:
        // template <typename Str, typename Handle_t>
        // void add_cmd(Str &&cmd_name, Handle_t &&handle);

        // template <typename... Args, typename Iter_t>
        // void exec(const Iter_t begin, const Iter_t end, Args&&... args) const;
    };

    // template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    // template <typename Str, typename Handle_t>
    // void Base_ctrl_console<Base_handle_t, endl_opt, endl, space>::add_cmd(Str &&cmd_name, Handle_t &&handle)
    // {
    //     if (cmds.find(cmd_name) == cmds.end())
    //     {
    //         cmds.emplace(std::forward<Str>(cmd_name),
    //                      Ctrl_handle(std::make_unique<Handle_t>(std::forward<Handle_t>(handle))));
    //     }
    //     else
    //     {
    //         throw std::logic_error{"The comand with specyfied name already exists!"};
    //     }
    // }

    // template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    // template <typename... Args, typename Iter_t>
    // void Base_ctrl_console<Base_handle_t, endl_opt, endl, space>::exec(const Iter_t begin, const Iter_t end, Args&&... args) const
    // {
    //     auto parsed_cmds = parser.parse(begin, end);

    //     std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this, &args...](auto &p_cmd)
    //                   {
    //                     if (cmds[p_cmd.name]->validate(p_cmd.arg))
    //                         cmds[p_cmd.name]->exec(std::forward<Args...>(args...), std::forward<std::decay_t<decltype(p_cmd.arg)>>(p_cmd.arg));
    //                     else
    //                         throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};   
    //                   });
    // }
}