#pragma once

#include <Server.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        class Impl_server final
        {
        public:
            static void set_options(User_opt &user, mqtt::connect_options &options);
        }
    }   
}
