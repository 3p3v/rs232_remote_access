#pragma once

#include <Dev_remover.hpp>
#include <Dev_adder.hpp>
#include <Dev_info.hpp>
#include <Impl_cont.hpp>
#include <impl/Controller.hpp>

using namespace Logic;

namespace Impl
{
    class Impl_adder final : private Dev_adder
    {
    public:
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    private:
        /// @brief 
        Impl_cont &infos;

        /// @brief 
        Mqtt_controller &controller;

    public:
        /// @brief 
        /// @param rec 
        /// @param info 
        void add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info);

        Impl_adder(Dev_cont &devs, Impl_cont &infos, Worker_storage& notyfier, Mqtt_controller &controller);
    };
} // namespace Impl
