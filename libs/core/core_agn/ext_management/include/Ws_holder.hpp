#pragma once

#include <Worker_storage.hpp>
#include <Base_ctrl_console.hpp>
#include <Remote_ext.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Ws_holder
    {
    protected:
        /// @brief Argument types for received command
        using Con_storage_param = Remote_ext::Command;

    private:
        using Console_base = Cmd_ctrl::Base_ctrl_console<Con_storage_param, Endl_opt::with>;

    public:
        Job_ctrl::Worker_storage ws{};
        Console_base cc{};

        Ws_holder() = default;
        Ws_holder(Ws_holder &&) = default;
        Ws_holder &operator=(Ws_holder &&) = default;
        Ws_holder(const Ws_holder &) = delete;
        Ws_holder &operator=(const Ws_holder &) = delete;
        virtual ~Ws_holder() = 0;
    };
}