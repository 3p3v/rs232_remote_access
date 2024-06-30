#pragma once

#include <Dev_remover.hpp>
#include <Dev_info.hpp>
#include <Impl_cont.hpp>
#include <Impl_ext.hpp>
#include <impl/Controller.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Interface for adding new devices
    class Impl_adder final : public Impl_ext
    {
    public:
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    private:
        /// @brief 
        Mqtt_controller &controller;

        /// @brief 
        Worker_storage& notyfier;

    public:
        /// @brief Add new device
        /// @param sett Devices local port settings
        /// @param write_access If user can send data to remote
        /// @param info Devices record
        void add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info);

        Impl_adder(Impl_cont &devs, Worker_storage& notyfier, Mqtt_controller &controller);
    };
} // namespace Impl
