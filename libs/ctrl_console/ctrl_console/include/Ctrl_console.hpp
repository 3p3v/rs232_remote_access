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
    
    template <typename Base_handle_t, Endl_opt endl_opt, char endl_ = '\n', char space_ = ' '>
    class Ctrl_console
        : private Ctrl_console_dummy<Base_handle_t, endl_opt, endl_, space_>,
          public Ctrl_con_storage<Base_handle_t, endl_opt, endl_, space_>,
          public Ctrl_con_exec<Base_handle_t, endl_opt, endl_, space_>
    {
    public:
        using Handle = Base_handle_t;

        Ctrl_console();
    };

    template <typename Base_handle_t, Endl_opt endl_opt, char endl_, char space_>
    inline Ctrl_console<Base_handle_t, endl_opt, endl_, space_>::Ctrl_console()
        : Ctrl_con_storage{Ctrl_console_dummy::cc}, Ctrl_con_exec{Ctrl_console_dummy::cc}
    {
    }
}