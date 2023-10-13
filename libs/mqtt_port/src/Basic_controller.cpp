#include <Basic_controller.hpp>

namespace Mqtt_port
{

    // Basic_controller::Basic_controller()
    //     : Receiver{validator}, 
    //       Sender{validator}, 
    //       Connector{validator}
    // {
    // }

    
    // void Basic_controller::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    // {
    //     sender.write(channel_name, data, write_len);
    // }


    void Basic_controller::run()
    {
        started = true;
        load_channels();
        run_exec();
    }

}