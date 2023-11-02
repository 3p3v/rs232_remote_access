#pragma once

#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Callb_t>
        class Callb_impl : public mqtt::iaction_listener
        {
        protected:
            using Grip = std::unique_ptr<Callb_t>;

        private:
            Grip &callb;

        protected:
            Callb_t &get_callb();

        public:
            Callb_impl(Grip &callb);

            void on_failure(const mqtt::token &asyncActionToken) = 0;

            void on_success(const mqtt::token &asyncActionToken) = 0;
        };

        template <typename Callb_t>
        Callb_impl<Callb_t>::Callb_impl(Grip &callb)
            : callb{callb}
        {
        }

        template <typename Callb_t>
        Callb_t &Callb_impl<Callb_t>::get_callb()
        {
            return *callb;
        }
    }
}