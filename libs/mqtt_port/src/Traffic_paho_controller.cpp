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

    
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
    void Traffic_paho_controller<T1, T2, T3, T4, T5, T6>::connect_callback(const std::string &cause)
    {
        /* Run user callback */
        connect_usr_callback(cause);
        
        /* Load all registered channels */
        load_channels(get_channels());

        /* Get one of the channels and subscribe to it */
        if (remaining_channels())
            subscribe(get_channel_to_con());
    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
    void Traffic_paho_controller<T1, T2, T3, T4, T5, T6>::connect()
    {

    }

    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
    void Traffic_paho_controller<T1, T2, T3, T4, T5, T6>::run()
    {
        // client.set_callback()
    }
}
