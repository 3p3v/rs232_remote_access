#pragma once

#include <Base_device_init.hpp>
/* IO implementation */
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
/* Notifications */
#include <Notification_manager.hpp>

using namespace Logic;

namespace Impl
{
    class Device_init final : public Base_device_init
    {
        Notification_manager &notifier;
        Mqtt_impl_factory remote_factory;
        Serial_impl_factory serial_factory;

    public:
        Device_init(Mqtt_impl_factory &&remote_factory, Serial_impl_factory &&serial_factory);

        std::shared_ptr<Base_proto_mediator> create(
            Notifier &&notifier,
            Proto_rec &&rec,
            Remote_rec &remote_rec,
            Serial_rec &serial_rec) override;
    };
}