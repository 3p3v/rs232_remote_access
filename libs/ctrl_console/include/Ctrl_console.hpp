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
        using Data = std::string;

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

    class Numbers_only : public Policy
    {
    public:
        bool validate(const std::string &arg) override
        {
            return true;
        }
    };

    // class Default : protected Common_defs
    // {
    // public:
    //     virtual void exec(const Data &arg) {};
    // };

    // class Extended : protected Common_defs
    // {
    // public:
    //     virtual void exec(const std::string& dev_name,const Data &arg) {};
    // };

    // template <typename... Args_t>

    class Mandatority
    {
    public:
        enum class Type
        {
            mandatory,
            optional
        };

    private:    
        bool executed{false};
        Type type{Type::optional};

    public:
        void set_mandatority(Type type)
        {
            this->type = type;
        }

        bool check()
        {
            if (type == Type::mandatory)
            {
                return executed;
            }
            else
            {
                return true;
            }
        }
    };

    template <typename... Args_t>
    class Exec
    {
    public:
        template <typename... Adds_t>
        class Addons
        {
        public:
            class Base_handle_intf : public Adds_t...
            {
            public:
                virtual bool validate(const std::string &arg) = 0;
                virtual void exec(Args_t... args) = 0;
            };

            template <typename... Policies_t>
            class Policies
            {
            public:
                class Base_handle : public Base_handle_intf
                {
                protected:
                    using Policy_ptr = std::unique_ptr<Policy>;
                    using Policy_cont = std::vector<Policy_ptr>;
                    Policy_cont policies;

                private:
                    template <typename... P>
                    Policy_cont make_policy_cont()
                    {
                        Policy_cont cont;
                        cont.emplace_back(Policy_ptr(new P{})...);
                        return cont;
                    }

                public:
                    Base_handle()
                        : policies(std::move(make_policy_cont<Policies_t...>()))
                    {
                    }

                    bool validate(const std::string &arg)
                    {
                        auto ret = std::any_of(policies.begin(), policies.end(),
                                               [&arg](Policy_ptr &policy)
                                               {
                                                   return policy->validate(arg);
                                               });

                        return ret;
                    }
                };

                template <typename Handle_f>
                class Dyn_handle : public Base_handle
                {
                    Handle_f handle;

                public:
                    Dyn_handle(Handle_f &&handle)
                        : handle{std::forward<Handle_f>(handle)}
                    {
                    }

                    void exec(Args_t... args)
                    {
                        handle(std::forward<Args_t>(args)...);
                    }
                };

                template <typename Handle_f>
                Dyn_handle(Handle_f &&) -> Dyn_handle<Handle_f>;

            };
        };
    };

    class Ctrl_cmd
    {
    public:
        std::string name{};
        std::string arg{};

        template <typename Str>
        Ctrl_cmd(Str &&name)
            : name{std::forward<Str>(name)}
        {
        }

        template <typename Iter>
        Ctrl_cmd(Iter begin, Iter end)
            : name{begin, end}
        {
        }

        template <typename Iter>
        Ctrl_cmd(Iter begin, Iter end, Iter a_begin, Iter a_end)
            : name{begin, end}, arg{a_begin, a_end}
        {
        }
    };

    class Ctrl_parser : protected Common_defs
    {
        using Ctrl_cmd_data_t = std::string;
        using Ctrl_cmd_data_name = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_arg = Ctrl_cmd_data_t;
        using Ctrl_cmd_data_pair = std::pair<Ctrl_cmd_data_name, Ctrl_cmd_data_arg>;
        using Ctrl_cmd_data_pair_con = std::vector<Ctrl_cmd_data_pair>;
        using Ctrl_cmd_pos_con = std::vector<Ctrl_cmd>;

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
            auto s_pos = std::find(s_begin, s_end, ' ');
            if (s_pos != s_end)
            {
                lines.emplace_back(s_begin, s_pos, s_pos + 1, s_end);
            }
            else
            {
                lines.emplace_back(s_begin, s_end);
            }
            /* Proceed to find next line */
            find_lines(lines, s_end + 1, end);
        }

    public:
        template <typename Iter_t>
        Ctrl_cmd_pos_con parse(const typename Iter_t begin, const typename Iter_t end)
        {
            Ctrl_cmd_pos_con lines;
            find_lines(lines, begin, end);
            return lines;
        }
    };

    class Ctrl_con_defs
    {
    public:
        static constexpr char endl = '\n';
        static constexpr char space = ' ';
    };

    template <typename Base_handle_t>
    class Base_ctrl_console : protected Common_defs, protected Ctrl_con_defs
    {
    protected:
        using Ctrl_cmd_name = std::string;
        using Ctrl_handle = std::unique_ptr<Base_handle_t>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_name,
                                             Ctrl_handle>;

        Cmds_cont cmds;
        Ctrl_parser parser;

    public:
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle)
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
    };

    namespace Setup
    {
        using Handle_t = Exec<std::string &&>::Addons<Mandatority>;

        class Setup_console final : public Base_ctrl_console<Handle_t::Base_handle_intf>
        {
        public:
            template <typename Iter_t>
            void exec(const typename Iter_t begin, const typename Iter_t end)
            {
                auto parsed_cmds = parser.parse(begin, end);

                std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
                              {
                    if (cmds[p_cmd.name]->validate(p_cmd.arg))
                        cmds[p_cmd.name]->exec(std::move(p_cmd.arg));
                    else
                        throw std::runtime_error("Received command: \"" + p_cmd.name + "\" didn't pass validation!"); });
            }

            void check()
            {
                std::for_each(cmds.begin(), cmds.end(), [](auto &cmd)
                              {
                    if (!cmd.second->check())
                    {
                        throw std::runtime_error("Value: " + cmd.first + " was not given!");
                    } });
            }
        };
    }

    namespace Ctrl
    {
        using Handle_t = Exec<const std::string &, const std::string &>::Addons<>;

        template <typename Local_exec_handler>
        class Ctrl_console final : public Base_ctrl_console<Handle_t::Base_handle_intf>
        {
            Local_exec_handler leh;

        public:
            Ctrl_console(Local_exec_handler &&leh)
                : leh{std::move(leh)}
            {
            }

            template <typename Iter_t>
            void exec(const std::string &device_name, const typename Iter_t begin, const typename Iter_t end)
            {
                auto parsed_cmds = parser.parse(begin, end);

                std::for_each(parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
                              {
                    if (cmds[p_cmd.name]->validate(p_cmd.arg))
                        cmds[p_cmd.name]->exec(device_name, std::move(p_cmd.arg));
                    else
                        throw std::runtime_error("Received command: \"" + p_cmd.name + "\" didn't pass validation!"); });
            }

            void local_exec(const std::string &info_ch, const std::string &name, const std::string &arg)
            {
                if (cmds[name]->validate(arg))
                {
                    leh(info_ch, name + space + arg + endl);
                }
            }
        };

        template <typename Local_exec_handler>
        Ctrl_console(Local_exec_handler &&) -> Ctrl_console<Local_exec_handler>;
    }
}