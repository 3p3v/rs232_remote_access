#pragma once

#include <Forwarder.hpp>

using namespace Job_ctrl;
using namespace Cmd_ctrl;

namespace Logic
{
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    class Remote_ext : public Worker
    {
    protected:
        virtual void add_restart_job() = 0;

    public:
        /// @brief Type of parameter used by all commands
        using Cmd_param = const std::string&;
        /// @brief Type of command used by extensions
        using Cmd = Exec<>::Param<Cmd_param>::Base_handle_intf;
        using Cmds_pack = std::vector<Cmd>;

        /// @brief Get all commands that extension uses to communicate with remote
        /// @return 
        virtual Cmds_pack get_cmds() = 0;

        template <typename Forwarder_ptr_t>
        Remote_ext(Forwarder_ptr_t&& manager);
        Remote_ext(const Remote_ext&) = delete;
        Remote_ext& operator=(const Remote_ext&) = delete;
        Remote_ext(const Remote_ext &) = default;
        Remote_ext& operator=(const Remote_ext&) = default;
        virtual ~Remote_ext() = 0;
    };

    template <typename Forwarder_ptr_t>
    inline Remote_ext::Remote_ext()
    {
        // Add job for at least resetting all timers (used when there was error in communication with device)
        add_restart_job();
    }
}