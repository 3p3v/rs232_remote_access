#pragma once

#include <User.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        class Impl_user final
        {
        public:
            static void set_options(User::Get_cont &&user, mqtt::connect_options &options);
        };
    }
}