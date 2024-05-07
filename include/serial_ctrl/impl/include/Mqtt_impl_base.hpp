#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <Mqtt_except.hpp>
// #include <Cmds_except.hpp>
#include <impl/Controller.hpp>
#include <Dev_info.hpp>

using namespace Logic;

namespace Impl
{
    class Mqtt_impl_base
    {
    public:
        using Dev_info_ptr = std::shared_ptr<Dev_info>;
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    protected:
        Dev_info_ptr info;

        /// @brief MQTT client implementaton
        Mqtt_controller &controller;

        /// @brief MQTT QOS
        static constexpr unsigned char qos{Topic_defs::qos};

        /// @brief 
        static constexpr std::string_view packet_num_s{Topic_defs::packet_num_s};

    public:
        template <typename Dev_info_ptr_t>
        Mqtt_impl_base(
            Dev_info_ptr_t &&device,
            Mqtt_controller &controller);
        
    };

    template <typename Dev_info_ptr_t>
    inline Mqtt_impl_base::Mqtt_impl_base(Dev_info_ptr_t &&device, Mqtt_controller &controller)
        : device{std::forward<Dev_info_ptr_t>(device)}, controller{controller}
    {
    }
}