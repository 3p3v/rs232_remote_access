#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace Cmd_ctrl
{
    class Common_defs
    {
    public:
        using Data = std::vector<unsigned char>;

        virtual ~Common_defs() = 0;
    };

    Common_defs::~Common_defs()
    {
    }

    class Policy
    {
    public:
        virtual bool validate(const std::string &arg) = 0;
    };

    class NumbersOnly : public Policy
    {
    public:
        bool validate(const std::string &arg) override
        {
            return true;
        }
    };

    class Base_handle : protected Common_defs
    {
    protected:
        using Policy_ptr = std::unique_ptr<Policy>;
        using Policy_cont = std::vector<Policy_ptr>;
        Policy_cont policies;

    public:
        Base_handle(Policy_cont &&policies)
            : policies(std::move(policies))
        {
        }

        bool validate(const std::string &arg)
        {
            std::for_each(policies.begin(), policies.end(),
                          [&arg](Policy_ptr &policy)
                          {
                              policy->validate(arg);
                          });
        }

        virtual void exec(const Data &arg) = 0;
    };

    template <typename... Policies_t>
    class Def_handle : public Base_handle
    {
        template <typename... P>
        Policy_cont make_policy_cont()
        {
            Policy_cont cont;
            cont.emplace_back(Policy_ptr(new P{})...);
            return cont;
        }

    public:
        Def_handle()
            : Base_handle{std::move(make_policy_cont<Policies_t...>())}
        {
        }

        void exec(const Data &arg) = 0;
    };

    template <typename Handle, typename... Policies_t>
    class Dyn_handle : public Def_handle<Policies_t...>
    {
        Handle handle;

    public:
        Dyn_handle(Handle &&handle)
            : handle{std::forward<Handle>(handle)}
        {
        }

        void exec(const Base_handle::Data &arg) override
        {
            handle(arg);
        }
    };

    template <typename Handle, typename... Policies_t>
    Dyn_handle(Handle &&) -> Dyn_handle<Handle, Policies_t...>;

    template <typename... Policies_t>
    class Policies
    {
    public:
        template <typename Handle>
        static decltype(auto) make_dyn_handle(Handle &&handle)
        {
            return Dyn_handle<Handle, Policies_t...>(std::forward<Handle>(handle));
        }
    };

    class Ctrl_cmd
    {
    public:
        std::string name{};
        std::vector<std::string> args{};
    };

    class Ctrl_parser : protected Common_defs
    {
        using Ctrl_cmd_data_t = std::string;
        using Ctrl_cmd_data_name = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_arg = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_pair = std::pair<Ctrl_cmd_data_name, Ctrl_cmd_data_arg>;
        using Ctrl_cmd_data_pair_con = std::vector<Ctrl_cmd_data_pair>;
        using Ctrl_cmd_pos_con = std::vector<std::vector<Ctrl_cmd_data_t>>;

        template <typename Cont_iter_t>
        void find_lines(Ctrl_cmd_pos_con &lines, const Cont_iter_t begin, const Cont_iter_t end)
        {
            /* Find if another line exists */
            if (begin == end)
            {
                /* No more lines to find, OK */
                return;
            }

            /* Find end of line */
            auto s_end = std::find(begin, end, '\n');
            if (s_end == end)
            {
                /* Found one more line but not correctly ended, error */
                throw std::runtime_error("Found not correctly formatted line!");
                return;
            }
            auto s_begin = begin;

            /* Add new cmd */
            lines.push_back(std::vector<std::string>{});

            /* Find all words in line */
            auto s_pos = std::find(s_begin, s_end, ' ');
            do
            {
                lines[lines.size() - 1].emplace_back(s_begin, s_pos);
                s_begin = s_pos + 1;
            } while ((s_pos = std::find(s_begin, s_end, ' ')) != s_end);

            /* Proceed to find next line */
            find_lines(lines, s_end + 1, end);               
        }

    public:
        Ctrl_cmd_pos_con parse(const Data &data)
        {
            Ctrl_cmd_pos_con lines;
            find_lines(lines, data.cbegin(), data.cend());
            return lines;
        }
    };

    class Ctrl_console final : protected Common_defs
    {
        using Ctrl_cmd_name = std::string;
        using Ctrl_handle = std::unique_ptr<Base_handle>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_pair::first_type,
                                             Ctrl_cmd_pair::second_type>;

        Cmds_cont cmds;
        Ctrl_parser parser;

        std::vector<std::string::iterator> find_words(Data::iterator begin, Data::iterator end)
        {
        }

    public:
        void exec(const Data &arg)
        {
        }

        template <typename Str, typename handle_t>
        void add_cmd(Str &&cmd_name, handle_t &&handle)
        {
            if (cmds.find(cmd_name) == cmds.end())
            {
                cmds.insert(Ctrl_cmd_pair{std::forward<Str>(cmd_name),
                                          Ctrl_handle(new handle_t{std::forward<handle_t>(handle)})});
            }
            else
            {
                throw std::logic_error{"The comand with specyfied name already exists!"};
            }
        }
    };
}