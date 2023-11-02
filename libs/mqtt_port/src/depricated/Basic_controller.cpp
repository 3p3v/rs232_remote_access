#include <Basic_controller.hpp>

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

    void Controller::run()
    {
        // started = true;
        if (!connector->load_channels())
            throw std::logic_error("No channels to connect were specyfied!");

        controller.connect(Conn_callb{controller, connector});
    }

    void Controller::disconnect(Time time)
    {
        controller.disconnect(time);
    }
}