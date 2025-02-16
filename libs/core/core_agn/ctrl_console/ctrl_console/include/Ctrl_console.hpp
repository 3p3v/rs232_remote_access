#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <Exec.hpp>
#include <Ctrl_parser.hpp>

namespace Cmd_ctrl
{
    template <typename Exec_t>
    class Ctrl_console
    {
        /// @brief Checks if template argument is a container
        template <typename T>
        struct is_container : std::integral_constant<
                                  bool,
                                  has_const_iterator<T>::value &&
                                      has_begin_end<T>::beg_value &&
                                      has_begin_end<T>::end_value>
        {
        };

    public:
        using Ctrl_cmd_pair = std::pair<
            std::string,
            Exec_t>;
        using Cmds_cont = std::unordered_map<
            std::string,
            Exec_t>;

        mutable Cmds_cont cmds;
        Ctrl_parser parser;

        template <typename Ctrl_parser_t>
        Ctrl_console(Ctrl_parser_t &&parser);

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

        /// @brief Add new command
        /// @tparam Str std::string or any other type that string can be constructed from
        /// @tparam Handle_t Function type void(Base_handle_t::Arg)
        /// @param cmd_name Command name
        /// @param handle Handler to be executed of type void(Base_handle_t::Arg)
        template <typename Str, typename Handle_t>
        void add_cmd(Str &&cmd_name, Handle_t &&handle);

        // /// @brief Add new command
        // /// @tparam Cmd_pair_t std::pair
        // /// @param cmd Command name & handler to be executed of type void(Base_handle_t::Arg)
        // template <typename Cmd_pair_t>
        // void add_cmd(Cmd_pair_t &&cmd);

        /// @brief Remove cmd with given name
        /// @tparam Str
        /// @param cmd_name
        template <typename Str>
        void remove_cmd(const Str &cmd_name);

        /// @brief Remove many cmds with given names
        /// @tparam Iter_t Iterator
        /// @param begin First command to be erased
        /// @param end Last + 1 command to be erased
        template <typename Iter_t>
        void remove_cmd(Iter_t begin, Iter_t end);

        template <typename Str>
        void enable_cmd(const Str &cmd_name, bool enabled);

        template <typename Str>
        void enable_cmd(const Str &cmd_name);

        template <typename Str>
        void disable_cmd(const Str &cmd_name);

        template <typename Str>
        void disable_all_but(const Str &cmd_name);

        template <typename Iter_t>
        void disable_all_but(Iter_t begin, Iter_t end) noexcept;

        template <
            typename Cont_t,
            typename = typename std::enable_if_t<is_container<Cont_t>::type>>
        void disable_all_but(Cont_t &elems) noexcept;

        template <typename Str>
        void disable_all_but(std::initializer_list<Str> &cmd_name) noexcept;

        void disable_all() noexcept;
    };

    template <typename Exec_t>
    template <typename Ctrl_parser_t>
    Ctrl_console<Exec_t>::Ctrl_console(Ctrl_parser_t &&parser)
        : parser{std::forward<Ctrl_parser_t>(parser)}
    {
    }

    template <typename Exec_t>
    template <typename Iter_t>
    void Ctrl_console<Exec_t>::exec(const Iter_t begin, const Iter_t end) const
    {
        auto parsed_cmds = cc.parser.parse(begin, end);

        std::for_each(
            parsed_cmds.begin(), parsed_cmds.end(), [this](auto &p_cmd)
            {
                auto itr = cc.cmds.find(p_cmd.name);
                
                if (itr != cc.cmds.end())
                {
                    if (itr->second.exec->validate(p_cmd.arg))
                    {
                        itr->second.exec->exec(std::move(p_cmd.arg));
                    }
                    else
                    {
                        throw std::logic_error{"Received command: \"" + p_cmd.name + "\" didn't pass validation!"};
                    }
                }
                else
                {
                    throw std::logic_error{"Command \"" + p_cmd.name + "\" not found!"};
                } });
    }

    template <typename Exec_t>
    template <typename Iter_t, typename Callb_t>
    void Ctrl_console<Exec_t>::exec(const Iter_t begin, const Iter_t end, const Callb_t &callb) const
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

    template <typename Exec_t>
    template <typename Str, typename Handle_t>
    void Ctrl_console<Exec_t>::add_cmd(Str &&cmd_name, Handle_t &&handle)
    {
        if (cc.cmds.find(cmd_name) == cc.cmds.end())
        {
            cc.cmds.emplace(
                std::forward<Str>(cmd_name),
                Base_ctrl_console::Exec_tuple(std::make_unique<Handle_t>(std::forward<Handle_t>(handle))));
        }
        else
        {
            throw std::logic_error{"The comand with specified name already exists!"};
        }
    }

    // template <typename Cmd_pair_t>
    // inline void Ctrl_console<Exec_t>::add_cmd(Cmd_pair_t &&cmd)
    // {
    //     if (cc.cmds.find(cmd.first) == cc.cmds.end())
    //     {
    //         cc.cmds.insert(std::forward<Cmd_pair_t>(cmd));
    //     }
    //     else
    //     {
    //         throw std::logic_error{"The comand with specified name already exists!"};
    //     }
    // }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::remove_cmd(const Str &cmd_name)
    {
        auto cmd_idx = cc.cmds.find(cmd_name);

        if (cmd_idx != cc.cmds.end())
        {
            cc.cmds.erase(cmd_idx);
        }
        else
        {
            throw std::logic_error{"The comand with specified name does not exists!"};
        }
    }

    template <typename Exec_t>
    template <typename Iter_t>
    inline void Ctrl_console<Exec_t>::remove_cmd(Iter_t begin, Iter_t end)
    {
        std::for_each(
            begin,
            end,
            [this](const auto &cmd_name)
            {
                auto cmd_idx = cc.cmds.find(cmd_name);

                if (cmd_idx != cc.cmds.end())
                {
                    cc.cmds.erase(cmd_idx);
                }
                else
                {
                    throw std::logic_error{"The comand with specified name does not exists!"};
                }
            });
    }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::enable_cmd(const Str &cmd_name, bool enabled)
    {
        auto cmd = cc.cmds.find(cmd_name);
        if (cmd != cc.cmds.end())
        {
            cmd->second.enable(enable);
        }
        else
        {
            throw std::logic_error{"The comand with specified name does not exist!"};
        }
    }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::enable_cmd(const Str &cmd_name)
    {
        enable_cmd(cmd_name, true);
    }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::disable_cmd(const Str &cmd_name)
    {
        enable_cmd(cmd_name, false);
    }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::disable_all_but(const Str &cmd_name)
    {
        /* Find unlockable first */
        auto cmd = cc.cmds.find(cmd_name);
        if (cmd != cc.cmds.end())
        {
            cmd->second.enable();
        }
        else
        {
            throw std::logic_error{"The comand with specified name does not exist!"};
        }

        /* Lock else */
        std::for_each(
            cc.cmds.begin(),
            cmd,
            [&cmd_name](auto &c)
            {
                c.second.disable();
            });
        std::for_each(
            cmd + 1,
            cc.cmds.end(),
            [&cmd_name](auto &c)
            {
                c.second.disable();
            });
    }

    template <typename Exec_t>
    template <typename Iter_t>
    inline void Ctrl_console<Exec_t>::disable_all_but(Iter_t begin, Iter_t end) noexcept
    {
        auto elems = std::vector<std::iterator_traits<Iter_t>::value_type>(begin, end);
        disable_all_but(elems);
    }

    template <typename Exec_t>
    template <
        typename Cont_t,
        typename>
    inline void Ctrl_console<Exec_t>::disable_all_but(Cont_t &elems) noexcept
    {
        std::for_each(
            cc.cmds.begin(),
            cc.cmds.end(),
            [&cmd_name, &elems](auto &c)
            {
                auto e = std::find(elems.begin(), elems.end(), c.first);
                if (e == elems.end())
                {
                    c.second.disable();
                }
                else
                {
                    c.second.enable();
                    elems.erase(e);
                }
            });
    }

    template <typename Exec_t>
    template <typename Str>
    inline void Ctrl_console<Exec_t>::disable_all_but(std::initializer_list<Str> &cmd_name) noexcept
    {
        auto elems = std::vector<str>(cmd_name);
        disable_all_but(elems);
    }

    template <typename Exec_t>
    void Ctrl_console<Exec_t>::disable_all() noexcept
    {
        std::for_each(
            cc.cmds.begin(),
            cc.cmds.end(),
            [](auto &c)
            {
                c.second.disable();
            });
    }
}