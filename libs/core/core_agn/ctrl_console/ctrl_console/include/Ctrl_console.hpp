#pragma once

#include <Ctrl_con_storage.hpp>
#include <Ctrl_con_exec.hpp>

namespace Cmd_ctrl
{
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '> 
    class Ctrl_console_dummy
    {
    public:
        Base_ctrl_console<Base_handle_t, endl_opt, endl_, space_> cc;
    };
    
    /// @brief Wrapper for all console functionality
    /// @tparam Base_handle_t Specyfication of Exec<... Args_t>::Param<Args>::Policies<... Policies_t>::Base_handle // TODO delete, this is unused and unnessesary
    /// @tparam endl_opt Checking endline enabled/disabled
    /// @tparam endl_ Character between different commands
    /// @tparam space_ Character between parameter name and argument 
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_console
        : private Ctrl_console_dummy<Base_handle_t, endl_opt, endl_, space_>,
          public Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>,
          public Ctrl_con_exec<Base_handle_t, endl_opt, endl_, space_>
    {
    public:
        using Handle = Base_handle_t;

        Ctrl_console();
        Ctrl_console(Ctrl_console &&) noexcept;
        Ctrl_console& operator=(Ctrl_console &&) = delete;
        Ctrl_console(const Ctrl_console &) = delete;
        Ctrl_console& operator=(const Ctrl_console &) = delete;
        ~Ctrl_console();
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    inline Ctrl_console<Base_handle_t, endl_opt, endl_, space_>::Ctrl_console()
        : Ctrl_con_storage{Ctrl_console_dummy::cc}, 
          Ctrl_con_exec{Ctrl_console_dummy::cc}
    {
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    inline Ctrl_console<Base_handle_t, endl_opt, endl_, space_>::Ctrl_console(Ctrl_console &&cc) noexcept
        : Ctrl_console_dummy{std::move(cc)},
          Ctrl_con_storage{Ctrl_console_dummy::cc}, 
          Ctrl_con_exec{Ctrl_console_dummy::cc}
    {
    }

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    inline Ctrl_console<Base_handle_t, endl_opt, endl_, space_>::~Ctrl_console() = default;
}