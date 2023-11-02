#pragma once

#include <Callb_impl.hpp>
#include <C_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Channel_conn_callb_impl : public Callb_impl<C_callb>
        {
        public:
            Channel_conn_callb_impl(Grip &channel_conn_callback);

            void on_failure(const mqtt::token &asyncActionToken) override;

            void on_success(const mqtt::token &asyncActionToken) override;
        };
    }
}