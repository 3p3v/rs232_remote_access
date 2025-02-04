#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <Exec.hpp>
#include <Ctrl_parser.hpp>

namespace Cmd_ctrl
{
    class Base_ctrl_console
    {
        class Exec_tuple
        {
            bool blocked{true};
            bool enabled{true};

        public:
            const std::unique_ptr<Base_exec> exec;
            
            void enable(bool enable);
            void enable();
            void disable();

            Exec_tuple(
                std::unique_ptr<Base_exec> &&exec, 
                bool enabled = true, 
                bool blocked = false);
        };

    public:
        using Ctrl_cmd_pair = std::pair<
            std::string, 
            Base_exec>;
        using Cmds_cont = std::unordered_map<
            std::string,
            Base_exec>;

        mutable Cmds_cont cmds;
        Ctrl_parser parser;

        template <typename Ctrl_parser_t>
        Base_ctrl_console(Ctrl_parser_t &&parser);
    };

    template <typename Ctrl_parser_t>
    Base_ctrl_console::Base_ctrl_console(Ctrl_parser_t &&parser)
        : parser{std::forward<Ctrl_parser_t>(parser)}
    {
    }
}