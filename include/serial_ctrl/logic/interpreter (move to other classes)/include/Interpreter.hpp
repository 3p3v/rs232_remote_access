#pragma once

#include <Exec.hpp>
#include <Base_ctrl_console.hpp>
#include <Ip_defs.hpp>
#include <Ip_hi.hpp>
#include <Ip_get.hpp>
#include <Ip_packet.hpp>

using namespace Cmd_ctrl;

namespace Logic
{
    /* MQTT port settings */
    template <typename Mqtt>
    class Mqtt_settings<Mqtt>;

    /* Serial port settings */
    template <typename Serial>
    class Serial_settings<Serial>;

    /* Generic command interpreter */
    template <typename Mqtt, typename Serial>
    using Console = Base_ctrl_console<Exec<Mqtt_side<Mqtt> &, Serial_side<Serial> &, const std::string &>, Endl_opt::with>;

    /// @brief MQTT commands interpreter
    /// @tparam Mqtt 
    /// @tparam Serial 
    template <typename Mqtt, typename Serial>
    class Interpreter : public Console<Mqtt, Serial>
    {
        using Command = Console::Handle;

    public:
        Interpreter();
    };
}