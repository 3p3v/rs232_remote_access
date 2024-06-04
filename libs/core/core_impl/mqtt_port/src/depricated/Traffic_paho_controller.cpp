#pragma once 

#include <Traffic_paho_controller.hpp>

namespace Mqtt_port
{
    // Traffic_paho_controller::Traffic_paho_controller(std::string server_address, 
    //                                                  mqtt::async_client client, 
    //                                                  mqtt::connect_options options)
    //     : server_address{server_address}, 
    // {
    // }

    
    void Traffic_paho_controller::connect_callback(const std::string &cause)
    {
        /* Run user callback */
        connect_usr_callback(cause);
        
        /* Load all registered channels */
        load_channels(get_channels());

        /* Get one of the channels and subscribe to it */
        if (remaining_channels())
            subscribe(get_channel_to_con());
    }

    void Traffic_paho_controller::connect()
    {

    }


    void Traffic_paho_controller::run()
    {
        if (user.)
        options.
        
        connect();
    }
}
