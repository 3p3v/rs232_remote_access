#include <Conn_callb.hpp>
#include <stdexcept>
#include <Channel_conn_callb.hpp>
#include <impl/Controller.hpp>

namespace Mqtt_port
{
    Conn_callb::Conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
        : C_callb{controller, std::move(connector)}
    {
    }

    void Conn_callb::fail()
    {
        throw std::runtime_error("Could not connect to broker!");
    }

    void Conn_callb::success()
    {
        // Connect to channels if there are any
        if (connector->remaining_channels())
            controller.subscribe(connector->get_channel_to_con(), qos, Channel_conn_callb{controller, connector});
    }

}