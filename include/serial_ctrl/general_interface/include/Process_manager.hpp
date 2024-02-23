#pragma once

// #include <Base_ctrl_con_exec.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <Ctrl_con_storage.hpp>
#include <Forwarder.hpp>
#include <Storage.hpp>
#include <Remote_ext.hpp>

using namespace Cmd_ctrl;
using namespace Job_ctrl;

namespace Logic
{
    using Worker_ref = Worker &;
    using Ext_forwarder = Forwarder<Worker_ref>;
    using Ext_storage = Storage<Worker_ref>;
    using Con_storage_param = Exec<>::Param<Remote_ext_base::Cmd_param>;

    using Console_storage = Ctrl_con_storage<Con_storage_param, Endl_opt::with>;

    /// @brief Basic needs of Logic extension
    class Process_manager
        : private Console_storage,
          private Ext_storage,
          public Ext_forwarder
    {
    protected:
        using Ext_ptr = std::shared_ptr<Remote_ext_base>;

        std::vector<Ext_ptr> exts;

    public:
        template <typename Remote_ext_t, typename... Args_t>
        void add_ext(Args_t &&...ext);
    };

    template <typename Remote_ext_t, typename... Args_t>
    inline void Process_manager::add_ext(Args_t &&...args)
    {
        auto ext_ptr = std::make_shared<Remote_ext_t>(std::forward<Args_t>(args)...);

        /* Commands for communcation between extensions */
        add_worker(ext_ptr.get());

        /* Commands for communcation between extensions and device */
        auto cmds = ext_ptr->get_cmds();
        std::for_each(
            cmds.begin(),
            cmds.end(),
            [this](auto &&cmd)
            {
                add_cmd(std::forward<decltype(std::decay_t(cmd))>(cmd));
            })
    }
}