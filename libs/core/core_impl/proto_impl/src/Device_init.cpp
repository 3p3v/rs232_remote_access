#include <Device_init.hpp>
/* Protocol framework */
#include <Proto_mediator.hpp>
#include <Proto_helpers.hpp>
/* Use mutexes for IO as using multiple threads */
#include <Locked_callback_wrapper.hpp>
/* Timer implementation */
#include <Custom_timer.hpp>
/* IO implementation */
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
/* Proto init */
#include <Device_init.hpp>
/* Notifier */
#include <Notifier.hpp>

using namespace Logic;

namespace Impl
{
    using Proto = Proto_mediator<
        Proto_helpers<
            Custom_timer_maker,
            Mqtt_impl,
            Serial_impl>,
        Locked_callback_wrapper>;

    Device_init::Device_init(
        Notifier_manager &notifier_manager,
        Mqtt_impl_factory &&remote_factory,
        Serial_impl_factory &&serial_factory)
        : notifier_manager{notifier_manager}, remote_factory{std::move(remote_factory)}, serial_factory{std::move(serial_factory)}
    {
    }

    std::shared_ptr<Base_proto_mediator> Device_init::create(
        Proto_rec &&rec,
        Remote_rec &remote_rec,
        Serial_rec &serial_rec)
    {
        auto device = std::make_shared<Proto>(
            Locked_callback_wrapper{},
            Notifier{notifier_manager},
            std::move(rec),
            std::move(remote_rec),
            std::move(serial_rec),
            remote_factory,
            serial_factory);
        Device_init{}.init(device);
        return device;
    }
}