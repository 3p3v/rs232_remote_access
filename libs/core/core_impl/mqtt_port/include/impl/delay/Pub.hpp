#pragma once

#include <Sub.hpp>
#include <memory>

namespace Mqtt_port
{
    namespace Impl
    {
        class Pub : public Sub
        {
        public:
            using Data = std::string;
            std::shared_ptr<Data> data;

            template <typename Str>
            Pub(Str &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data, std::unique_ptr<mqtt::iaction_listener> &&callb);
            template <typename Str>
            Pub(Str &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data);
            Pub(Pub&&) = default;
            Pub& operator=(Pub&&) = default;
            Pub(Pub&) = delete;
            Pub& operator=(Pub&) = delete;
            ~Pub() = default;
        };  

        template <typename Str>
        Pub::Pub(Str &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data, std::unique_ptr<mqtt::iaction_listener> &&callb)
            : Sub{std::forward<Str>(channel_name), qos, std::move(callb)}, data(std::move(data))
        {

        }

        template <typename Str>
        Pub::Pub(Str &&channel_name, unsigned char qos, std::shared_ptr<Data> &&data)
            : Sub{std::forward<Str>(channel_name), qos}, data(std::move(data))
        {
        }
    }
}
