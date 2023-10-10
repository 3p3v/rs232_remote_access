#pragma once

#include <Traffic_paho_action_listener.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    class Traffic_mqtt_callback : public virtual mqtt::callback
    {
        Con_lost_func con_lost_callback;
        Con_func con_callback;
        Err_func error_callback;
        Func success_callback;

        Traffic_mqtt_action_listener<Err_func, Func> sub;

    public:
        void connection_lost(const std::string &cause) override;
        void connected(const std::string &cause) override;

        Traffic_mqtt_callback(Con_lost_func con_lost_callback, Con_func con_callback,
                              Err_func error_callback, Func success_callback);
    };

    template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    Traffic_mqtt_callback<Con_lost_func, Con_func, Err_func, Func>::
        Traffic_mqtt_callback(Con_lost_func con_lost_callback, Con_func con_callback,
                              Err_func error_callback, Func success_callback)
        : con_lost_callback{con_lost_callback}, con_callback{con_callback},
          error_callback{error_callback}, success_callback{success_callback},
          sub{error_callback, success_callback}
    {
    }

    template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    void Traffic_mqtt_callback<Con_lost_func, Con_func, Err_func, Func>::
        connection_lost(const std::string &cause)
    {
        con_lost_callback(cause);
    }

    template <typename Con_lost_func, typename Con_func, typename Err_func, typename Func>
    void Traffic_mqtt_callback<Con_lost_func, Con_func, Err_func, Func>::
        connected(const std::string &cause)
    {
        con_callback(cause);
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