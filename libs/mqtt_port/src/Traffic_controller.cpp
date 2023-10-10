#include <Traffic_controller.hpp>

namespace Mqtt_port
{
    void Traffic_controller::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        validate(channel_name);
        write_exec(channel_name, data, write_len);
    }

    // unsigned int Traffic_controller::connect_channels()
    // {
        
    // }

    Traffic_controller::~Traffic_controller()
    {
    }
}