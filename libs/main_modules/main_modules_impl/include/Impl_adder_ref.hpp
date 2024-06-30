#pragma once

#include <Dev_info.hpp>
#include <Impl_cont.hpp>
#include <Impl_adder_ref_base.hpp>
#include <impl/Controller.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Interface for adding new devices
    class Impl_adder_ref final : public Impl_adder_ref_base
    {
    public:
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    private:
        /// @brief Low-level MQTT driver
        Mqtt_controller &controller;

    public:
        /// @brief Add new device
        /// @param sett Devices local port settings
        /// @param write_access If user can send data to remote
        /// @param info Devices record
        void add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info, Impl_cont &devs, Worker_storage& notyfier) override;

        Impl_adder_ref(Mqtt_controller &controller);
    };
} // namespace Impl
