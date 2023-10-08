#include <Traffic_controller.hpp>

namespace Mqtt_port
{
    void Traffic_controller::write(const std::string &channel_name, const Data_type &data)
    {
        validate(channel_name);
        write_exec(channel_name, data);
    }
}