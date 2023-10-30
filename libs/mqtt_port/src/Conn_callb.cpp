#include <Conn_callb.hpp>

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
        if (!connector->remaining_channels())
            throw std::logic_error("No channels to connect were specyfied!");
        else
            controller.subscribe(connector->get_channel_to_con(), qos, Channel_conn_callb{controller, connector});
    }

}