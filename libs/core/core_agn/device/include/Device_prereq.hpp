#pragma once

#include <Notyfier.hpp>
#include <Remote_dev.hpp>
#include <Ctrl_console.hpp>
#include <Proto_module.hpp>
#include <Greeter.hpp>
/* Message count */
#include <Packet_defs.hpp>
#include <Packet_master.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Packet_slave.hpp>
#include <Packet_controller.hpp>
#include <Packet_sett_final.hpp>

namespace Logic
{
    /// @brief Dummy for storing in container
    class Device_prereq
    {
    protected:///???? TODO move??
        /// @brief Exchanger module
        std::unique_ptr<Proto_module> exchanger; // TODO: make mediator??

        std::unique_ptr<Greeter> greeter; // TODO: make mediator??

    public:
        /// @brief Timer manager
        Timer_cont timer_man;

        /// @brief User notification
        Notifier notifier;

        /// @brief Device record
        Remote_dev rec;

        /// @brief Container for all commands
        Cmd_ctrl::Ctrl_console cmds;

        /// @brief 
        /// @tparam Device_ptr_t Device_weak_ptr
        /// @param notifier User notification
        /// @param dev Pointer to device holder (Device_base / Device_initializer)
        /// @param rec Device record object
        template <typename Device_ptr_t>
        Device_prereq(
            Notifier &&notifier,
            Remote_dev &&rec);

        Device_prereq(Device_prereq &&) = default;
        Device_prereq &operator=(Device_prereq &&) = default;
        Device_prereq(const Device_prereq &) = default;
        Device_prereq &operator=(const Device_prereq &) = default;
        virtual ~Device_prereq() = default;
    };

    Device_prereq::Device_prereq(
        Notifier &&notifier,
        Remote_dev &&rec,
        std::unique_ptr<Proto_module>&& exchanger)
        : notifier{std::move(notifier)},
          rec_(std::move(rec)),
          exchanger{std::move(exchanger)}
    {
    }
}