#pragma once

#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    class Traffic_mqtt_action_listener : public virtual mqtt::iaction_listener
    {
        // std::string action_name;

    protected:
        void on_failure(const mqtt::token &tok) override final;
        void on_success(const mqtt::token &tok) override final;

    // protected:
    //     virtual void error_callback(const std::string &channel_name, unsigned int message_id) = 0;

    public:
        Traffic_mqtt_action_listener();
    };

    Traffic_mqtt_action_listener::Traffic_mqtt_action_listener()
        // : error_callback{error_callback}, success_callback{success_callback}
    {
    }

    void Traffic_mqtt_action_listener::on_failure(const mqtt::token &tok)
    {
        // error_callback(tok.get_return_code(), tok.get_message_id());
    }

    void Traffic_mqtt_action_listener::on_success(const mqtt::token &tok)
    {
        // success_callback(tok.get_return_code(), tok.get_message_id());
    }
}