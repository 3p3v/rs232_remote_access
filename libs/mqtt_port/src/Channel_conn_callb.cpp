#include <Channel_conn_callb.hpp>
#include <impl/Controller.hpp>

namespace Mqtt_port
{
    Channel_conn_callb::Channel_conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
        : C_callb{controller, std::move(connector)}
    {
    }

    void Channel_conn_callb::fail()
    {
        throw std::runtime_error("Could not subscribe to channel!");
        controller.disconnect(time_to_disconnect);
    }

    void Channel_conn_callb::success()
    {
        if (connector->remaining_channels())
            controller.subscribe(connector->get_channel_to_con(), qos);
    }

}