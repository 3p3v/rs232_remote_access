#pragma once

#include <Base_ctrl_console.hpp>

namespace Cmd_ctrl
{
    /// @brief Interface for adding commands
    class Ctrl_con_storage
    {
        /// @brief Checks if template argument is a container
        template<typename T> 
        struct is_container : 
            std::integral_constant<
                bool, 
                has_const_iterator<T>::value && 
                has_begin_end<T>::beg_value && 
                has_begin_end<T>::end_value> 
        {
        };

        Base_ctrl_console &cc;

    public:
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
        void Ctrl_con_storage::disable_all_but(Cont_t &elems) noexcept;

        template <typename Str>
        void disable_all_but(std::initializer_list<Str> &cmd_name) noexcept;

        void disable_all() noexcept;

        Ctrl_con_storage(Base_ctrl_console &cc);
    };

    template <typename Str, typename Handle_t>
    void Ctrl_con_storage::add_cmd(Str &&cmd_name, Handle_t &&handle)
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
    // inline void Ctrl_con_storage::add_cmd(Cmd_pair_t &&cmd)
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

    template <typename Str>
    inline void Ctrl_con_storage::remove_cmd(const Str &cmd_name)
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

    template <typename Iter_t>
    inline void Ctrl_con_storage::remove_cmd(Iter_t begin, Iter_t end)
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

    template <typename Str>
    inline void Ctrl_con_storage::enable_cmd(const Str &cmd_name, bool enabled)
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

    template <typename Str>
    inline void Ctrl_con_storage::enable_cmd(const Str &cmd_name)
    {
        enable_cmd(cmd_name, true);
    }

    template <typename Str>
    inline void Ctrl_con_storage::disable_cmd(const Str &cmd_name)
    {
        enable_cmd(cmd_name, false);
    }

    template <typename Str>
    inline void Ctrl_con_storage::disable_all_but(const Str &cmd_name)
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
            }
        );
        std::for_each(
            cmd + 1,
            cc.cmds.end(),
            [&cmd_name](auto &c)
            {
                c.second.disable();
            }
        );
    }

    template <typename Iter_t>
    inline void Ctrl_con_storage::disable_all_but(Iter_t begin, Iter_t end) noexcept
    {
        auto elems = std::vector<std::iterator_traits<Iter_t>::value_type>(begin, end);
        disable_all_but(elems);
    }

    template <
        typename Cont_t,
        typename>
    inline void Ctrl_con_storage::disable_all_but(Cont_t &elems) noexcept
    {
        std::for_each(
            cc.cmds.begin(),
            cc.cmds.end(),
            [&cmd_name, &elems](auto &c)
            {
                auto e  = std::find(elems.begin(), elems.end(), c.first);
                if (e == elems.end())
                {
                    c.second.disable();
                }
                else
                {
                    c.second.enable();
                    elems.erase(e);
                }
            }
        );
    }

    template <typename Str>
    inline void Ctrl_con_storage::disable_all_but(std::initializer_list<Str> &cmd_name) noexcept
    {
        auto elems = std::vector<str>(cmd_name);
        disable_all_but(elems);
    }
}