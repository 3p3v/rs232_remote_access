#pragma once

#include <unordered_map>
#include <Ctrl_con_storage.hpp>
#include <Ctrl_con_exec.hpp>
#include <Exec.hpp>

using namespace Cmd_ctrl;

namespace Impl
{
    /// @brief Config interpreter
    class Setup_console
    {
    public:
        enum class Mandatoriness : bool
        {
            Mandatory = true,
            Optional = false
        };

        /// @brief Type of parameter used by all commands
        using Cmd_param = std::string &&;
        /// @brief Type of command used by extensions
        using Cmd = Exec<>::Param<Cmd_param>::Base_handle_intf;
        using Command = Exec<>::Param<Cmd_param>;

    private:
        Base_ctrl_console<Cmd, Endl_opt::without> cc;
        Ctrl_con_storage<Cmd, Endl_opt::without> c_storage;
        Ctrl_con_exec<Cmd, Endl_opt::without> c_exec;

        class Stat
        {
        public:
            Mandatoriness mandatoriness{Mandatoriness::Optional};
            bool executed{false};

            Stat() = default;
            Stat(Mandatoriness mandatoriness);
        };
        
        mutable std::unordered_map<std::string, Stat> run_stat;

        void mark(const std::string &name) const noexcept;

    public:
        /// @brief Add non-mandatory command
        /// @tparam Str_t 
        /// @tparam Handle_t 
        /// @param cmd_name Commad name
        /// @param handle Functor of type void(Cmd_param)
        template <typename Str_t, typename Handle_t>
        void add_cmd(Str_t &&cmd_name, Handle_t &&handle);

        /// @brief Add command with specyfied mandatoriness
        /// @tparam Str_t 
        /// @tparam Handle_t 
        /// @param cmd_name Commad name
        /// @param mandatoriness Mandatoriness
        /// @param handle Functor of type void(Cmd_param)
        template <typename Str_t, typename Handle_t>
        void add_cmd(Str_t &&cmd_name, Mandatoriness mandatoriness, Handle_t &&handle);

        /// @brief Execute commands from some string type
        /// @tparam Iter_t 
        /// @param begin 
        /// @param end 
        template <typename Iter_t>
        void exec(const Iter_t begin, const Iter_t end) const;

        /// @brief Check if all mandatory commands were executed
        void check();

        Setup_console();
    };

    template <typename Str_t, typename Handle_t>
    inline void Setup_console::add_cmd(Str_t &&cmd_name, Handle_t &&handle)
    {
        add_cmd(std::forward<Str_t>(cmd_name), Mandatoriness::Optional, std::forward<Handle_t>(handle));
    }

    template <typename Str_t, typename Handle_t>
    inline void Setup_console::add_cmd(Str_t &&cmd_name, Mandatoriness mandatoriness, Handle_t &&handle)
    {
        run_stat.emplace(cmd_name, mandatoriness);

        c_storage.add_cmd(std::forward<Str_t>(cmd_name), std::forward<Handle_t>(handle));
    }

    template <typename Iter_t>
    inline void Setup_console::exec(const Iter_t begin, const Iter_t end) const
    {
        c_exec.exec(
            begin, 
            end, 
            [this](const auto &name)
            {
                mark(name);
            });
    }
}