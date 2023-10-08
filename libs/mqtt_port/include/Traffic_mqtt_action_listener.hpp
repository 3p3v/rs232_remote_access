#pragma once

#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    template<typename Func>
    class Traffic_mqtt_action_listener : public virtual mqtt::iaction_listener
    {
        Func error_callback;
        std::string action_name;

        void on_failure(const mqtt::token &tok) override final;
        void on_success(const mqtt::token &tok) override final;

    // protected:
    //     virtual void error_callback(const std::string &channel_name, unsigned int message_id) = 0;

    public:
        action_listener(std::string action_name);
    };
}