#pragma once

#include <Traffic_mqtt_action_listener.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    class Traffic_mqtt_callback : public virtual mqtt::callback
    {

    public:
        void connection_lost(const std::string &cause) override;
        void connected(const std::string &cause) override;

        Traffic_mqtt_callback();
    };

    Traffic_mqtt_callback::
        Traffic_mqtt_callback()
    {
    }

    void Traffic_mqtt_callback::
        connection_lost(const std::string &cause)
    {
        // con_lost_callback(cause);
    }

    void Traffic_mqtt_callback::
        connected(const std::string &cause)
    {
        // con_callback(cause);
    }

    // template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    // void Traffic_mqtt_action_listener<Err_func, Func>::on_failure(const mqtt::token &tok)
    // {
    //     error_callback(tok.get_return_code(), tok.get_message_id());
    // }

    // template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    // void Traffic_mqtt_action_listener<Err_func, Func>::on_success(const mqtt::token &tok)
    // {
    //     success_callback(tok.get_return_code(), tok.get_message_id());
    // }
}