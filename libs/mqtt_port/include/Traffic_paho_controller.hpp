#pragma once

#include <Traffic_mqtt_controller.hpp>
#include <Traffic_paho_action_listener.hpp>
#include <Traffic_paho_callback.hpp>
#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
    class Traffic_paho_controller : public Traffic_mqtt_controller
    {
        mqtt::async_client client;
        mqtt::connect_options options;

        Traffic_mqtt_action_listener<T1, T2> pub{write_err_usr_callback, write_usr_callback};
        Traffic_mqtt_callback<T3, T4, T5, T6> con_sub;

    protected:
        void connect_callback(const std::string &cause) override;

    public:
        // template<
        template <typename Str>
        Traffic_paho_controller(Str &&server_address,
                                Traffic_mqtt_user &&user,
                                Traffic_mqtt_ssh &&ssh);
        // Traffic_paho_controller(std::string server_address, mqtt::async_client client, mqtt::connect_options options);
        // Traffic_paho_controller(std::string server_address, mqtt::async_client client, mqtt::connect_options options);
        /* No client ID */
        // Traffic_paho_controller(std::string server_address);

        void connect() override;
        void run() override;
    };
// <T1, T2, T3, T4, T5, T6>
    template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
    template <typename Str>
    Traffic_paho_controller<T1, T2, T3, T4, T5, T6>::Traffic_paho_controller(Str &&server_address,
                                                     Traffic_mqtt_user &&user,
                                                     Traffic_mqtt_ssh &&ssh)
        : Traffic_mqtt_controller{std::forward<Str>(server_address),
                                  std::forward<Traffic_mqtt_user>(user),
                                  std::forward<Traffic_mqtt_ssh>(ssh)},
          pub{write_err_usr_callback, write_usr_callback},
          con_sub{connect_err_usr_callback, connect_callback, read_err_usr_callback, read_usr_callback}
    {
    }
}