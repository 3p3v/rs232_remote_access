#pragma once

#include <string>
#include <memory>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        /// @brief Class used to store all information about subscribe
        class Sub
        {
        public:
            std::string channel_name;
            const unsigned char qos;
            std::unique_ptr<mqtt::iaction_listener> callb{nullptr};

            bool util_callb();

            Sub(std::string &&channel_name, unsigned char qos, std::unique_ptr<mqtt::iaction_listener> &&callb);
            Sub(std::string &&channel_name, unsigned char qos);
            Sub(Sub&&) = default;
            Sub& operator=(Sub&&) = default;
            Sub(Sub&) = delete;
            Sub& operator=(Sub&) = delete;
            virtual ~Sub() = default;
        };
    }
}
