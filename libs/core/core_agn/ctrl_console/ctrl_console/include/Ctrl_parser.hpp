#pragma once

#include <string>
#include <vector>
#include <Ctrl_cmd.hpp>

namespace Cmd_ctrl
{
    enum class Endl_opt
    {
        without,
        with
    };

    template <Endl_opt endl_opt, char endl = '\n', char space = ' '>
    class Ctrl_parser
    {
        using Ctrl_cmd_data_t = std::string;
        using Ctrl_cmd_data_name = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_arg = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_pair = std::pair<Ctrl_cmd_data_name, Ctrl_cmd_data_arg>;
        using Ctrl_cmd_data_pair_con = std::vector<Ctrl_cmd_data_pair>;
        using Ctrl_cmd_pos_con = std::vector<Ctrl_cmd>;

        template <typename Cont_iter_t>
        void find_lines(Ctrl_cmd_pos_con &lines, const Cont_iter_t begin, const Cont_iter_t end) const;

    public:
        template <typename Iter_t>
        Ctrl_cmd_pos_con parse(const Iter_t begin, const Iter_t end) const
        {
            // if (begin == end)
            // {
            //     throw std::logic_error{"No data!"};
            // }
            
            Ctrl_cmd_pos_con lines;
            find_lines(lines, begin, end);
            return lines;
        }
    };

    template <Endl_opt endl_opt, char endl, char space>
    template <typename Cont_iter_t>
    void Ctrl_parser<endl_opt, endl, space>::find_lines(Ctrl_cmd_pos_con &lines, const Cont_iter_t begin, const Cont_iter_t end) const
    {
        /* No endl specyfied */
        if constexpr (endl_opt == Endl_opt::without)
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
            /* Find end of line */
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
                find_lines(lines, s_end, end);
            }
        }
    }
}