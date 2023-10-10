#pragma once

#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    template<typename Err_func, typename Func>
    class Traffic_mqtt_action_listener : public virtual mqtt::iaction_listener
    {
        Err_func error_callback;
        Func success_callback;
        // std::string action_name;

    protected:
        void on_failure(const mqtt::token &tok) override final;
        void on_success(const mqtt::token &tok) override final;

    // protected:
    //     virtual void error_callback(const std::string &channel_name, unsigned int message_id) = 0;

    public:
        Traffic_mqtt_action_listener(Err_func error_callback, Func success_callback);
    };

    template<typename Err_func, typename Func>
    Traffic_mqtt_action_listener<Err_func, Func>::Traffic_mqtt_action_listener(Err_func error_callback, 
                                                                               Func success_callback)
        : error_callback{error_callback}, success_callback{success_callback}
    {
    }

    template<typename Err_func, typename Func>
    void Traffic_mqtt_action_listener<Err_func, Func>::on_failure(const mqtt::token &tok)
    {
        error_callback(tok.get_return_code(), tok.get_message_id());
    }

    template<typename Err_func, typename Func>
    void Traffic_mqtt_action_listener<Err_func, Func>::on_success(const mqtt::token &tok)
    {
        success_callback(tok.get_return_code(), tok.get_message_id());
    }
}