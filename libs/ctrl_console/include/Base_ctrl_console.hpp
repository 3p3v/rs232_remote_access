#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <Ctrl_con_defs.hpp>
#include <Common_defs.hpp>
#include <Ctrl_parser.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Base_ctrl_console : protected Common_defs//, protected Ctrl_con_defs
    {
    protected:
        using Ctrl_cmd_name = std::string;
        using Ctrl_handle = std::unique_ptr<Base_handle_t>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_name,
                                             Ctrl_handle>;

        static constexpr char endl = endl_;
        static constexpr char space = space_;

        Cmds_cont cmds;
        Ctrl_parser<endl_opt, endl, space> parser;

    public:
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle);
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl, char space>
    template <typename Str, typename Handle_t>
    void Base_ctrl_console<Base_handle_t, endl_opt, endl, space>::add_cmd(Str &&cmd_name, Handle_t &&handle)
    {
        if (cmds.find(cmd_name) == cmds.end())
        {
            cmds.emplace(std::forward<Str>(cmd_name),
                         Ctrl_handle(new Handle_t{std::forward<Handle_t>(handle)}));
        }
        else
        {
            throw std::logic_error{"The comand with specyfied name already exists!"};
        }
    }
}