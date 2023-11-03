#pragma once

#include <Sub.hpp>
#include <memory>

namespace Mqtt_port
{
    namespace Impl
    {
        class Pub : public Sub
        {
            using Data = std::string;
            std::shared_ptr<Data> data;
        public:
            Pub(std::string &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data, std::unique_ptr<mqtt::iaction_listener> &&callb);
            Pub(std::string &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data);
            Pub(Pub&&) = default;
            Pub& operator=(Pub&&) = default;
            Pub(Pub&) = delete;
            Pub& operator=(Pub&) = delete;
            ~Pub() = default;
        };    
    }
}
