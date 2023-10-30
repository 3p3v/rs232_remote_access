#include <Controller.hpp>

namespace Mqtt_port
{
    void Controller::run()
    {
        if (!connector->load_channels())
            throw std::logic_error("No channels to connect were specyfied!");

        controller.connect(Conn_callb{controller, connector});
    }

    void Controller::disconnect(Time time)
    {
        controller.disconnect(time);
    }
}