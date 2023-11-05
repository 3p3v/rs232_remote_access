#include <Controller.hpp>

namespace Mqtt_port
{
    void Controller::run()
    {
        // if (!connector->load_channels())
        //     throw std::logic_error("No channels to connect were specyfied!");

        controller.connect(Conn_callb{controller, connector});
    }

    void Controller::disconnect(Time time)
    {
        controller.disconnect(time);
    }
    // void Controller::connect_channels()
    // {
    //     if (connector->remaining_channels())
    //         controller.subscribe(connector->get_channel_to_con(), qos, Channel_conn_callb{controller, connector});
    // }
}