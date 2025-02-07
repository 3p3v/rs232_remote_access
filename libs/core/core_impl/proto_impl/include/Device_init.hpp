#pragma once

#include <Base_device_init.hpp>
/* IO implementation */
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
/* Notifications */
#include <Notifier_manager.hpp>

using namespace Logic;

namespace Impl
{
    class Device_init final : public Base_device_init
    {
        Mqtt_impl_factory remote_factory;
        Serial_impl_factory serial_factory;
        Notifier_manager &notifier_manager;

    public:
        Device_init(Notifier_manager &notifier_manager, Mqtt_impl_factory &&remote_factory, Serial_impl_factory &&serial_factory);

        std::shared_ptr<Base_proto_mediator> create(
            Proto_rec &&rec,
            Remote_rec &remote_rec,
            Serial_rec &serial_rec) override;
    };
}