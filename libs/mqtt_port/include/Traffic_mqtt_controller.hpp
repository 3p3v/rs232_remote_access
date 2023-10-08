#pragma once

#include <Traffic_controller.hpp>
#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    class Traffic_mqtt_controller final : public Traffic_controller
    {
        mqtt::async_client client;
        mqtt::connect_options
    public:
        Traffic_mqtt_controller(std::string server_address, std::string client_id);
        /* No client ID */
        Traffic_mqtt_controller(std::string server_address);
    };
}