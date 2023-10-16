#pragma once

#include <User.hpp>
#include <memory>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        class User_set_opt final// : public Mqtt_port::User_set_opt
        {
            // std::shared_ptr<mqtt::async_client> client;

        public:
            // User_set_opt(std::shared_ptr<mqtt::async_client> client);

            static void set_options(User_opt &user, mqtt::connect_options &options);
        };
    

        
    }
}