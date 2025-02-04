#pragma once

#include <string>
#include <vector>
#include <Ctrl_cmd.hpp>

namespace Cmd_ctrl
{
    class Ctrl_parser
    {
    public:
        enum class Endl_opt
        {
            without,
            with
        };
    
    private:
        using Ctrl_cmd_data_pair = std::pair<std::string, std::string>;
        using Ctrl_cmd_data_pair_con = std::vector<Ctrl_cmd_data_pair>;
        using Ctrl_cmd_pos_con = std::vector<Ctrl_cmd>;

        template <typename Cont_iter_t>
        void find_lines(Ctrl_cmd_pos_con &lines, Cont_iter_t begin, Cont_iter_t end) const;
        
        template <typename Cont_iter_t>
        void find_lines_with(Ctrl_cmd_pos_con &lines, Cont_iter_t begin, Cont_iter_t end) const;

        Endl_opt endl_opt = Endl_opt::with; 
        char endl = '\n'; 
        char space = ' ';

    public:
        template <typename Iter_t>
        Ctrl_cmd_pos_con parse(Iter_t begin, Iter_t end) const;

        Ctrl_parser();
        Ctrl_parser(Endl_opt endl_opt, char endl = '\n', char space = ' ');
    };

    Ctrl_parser::Ctrl_parser() = default;

    Ctrl_parser::Ctrl_parser(Endl_opt endl_opt = Endl_opt::with, char endl = '\n', char space = ' ')
        : endl_opt{endl_opt}, endl{endl}, space{space}
    {
    }

    template <typename Iter_t>
    Ctrl_parser::Ctrl_cmd_pos_con 
    Ctrl_parser::parse(Iter_t begin, Iter_t end) const
    {
        Ctrl_cmd_pos_con lines;
        find_lines(lines, begin, end);
        return lines;
    }

    template <typename Cont_iter_t>
    void Ctrl_parser::find_lines_with(Ctrl_cmd_pos_con &lines, Cont_iter_t begin, Cont_iter_t end) const
    {
        auto s_end = std::find(begin, end, endl);
        if (s_end == end)
        {
            /* Found one more line but not correctly ended, error */
            throw std::runtime_error("Found not correctly formatted line!");
        }
        auto s_begin = begin;

        /* Add new cmd */
        auto s_pos = std::find(s_begin, s_end, space);
        if (s_pos != s_end)
        {
            lines.emplace_back(s_begin, s_pos, s_pos + 1, s_end);
        }
        else
        {
            lines.emplace_back(s_begin, s_end);
        }

        if (++s_end == end)
        {
            /* All commands read */
            return;
        }
        else
        {
            /* Proceed to find next line */
            find_lines_with(lines, s_end, end);
        }
    }

    template <typename Cont_iter_t>
    void Ctrl_parser::find_lines(Ctrl_cmd_pos_con &lines, Cont_iter_t begin, Cont_iter_t end) const
    {
        /* No endl specyfied */
        if (endl_opt == Endl_opt::without)
        {
            auto s_begin = begin;
            auto s_end = end;

            /* Add new cmd */
            auto s_pos = std::find(s_begin, s_end, space);
            if (s_pos != s_end)
            {
                lines.emplace_back(s_begin, s_pos, s_pos + 1, s_end);
            }
            else
            {
                lines.emplace_back(s_begin, s_end);
            }

            return;
        }
        /* Endl specyfied, find it */
        else
        {
            find_lines_with(lines, begin, end);
        }
    }
}