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

    public:
        using Ctrl_cmd_name = std::string;
        using Ctrl_handle = std::unique_ptr<typename Base_handle_t::Base_handle_intf>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_name,
                                             Ctrl_handle>;

        static constexpr char endl = endl_;
        static constexpr char space = space_;

        mutable Cmds_cont cmds;
        Ctrl_parser<endl_opt, endl, space> parser;
    };
}