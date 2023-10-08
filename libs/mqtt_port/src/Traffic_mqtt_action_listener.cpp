#include <Traffic_mqtt_action_listener.hpp>

namespace Mqtt_port
{
    Traffic_mqtt_action_listener::action_listener(std::string action_name)
        : action_name(std::move(action_name)) 
    {
    }

    void Traffic_mqtt_action_listener::on_failure(const mqtt::token &tok)
    {
        error_callback(tok.get_return_code(), tok.get_message_id());
    }

    void Traffic_mqtt_action_listener::on_success(const mqtt::token &tok)
    {
        
    }
}