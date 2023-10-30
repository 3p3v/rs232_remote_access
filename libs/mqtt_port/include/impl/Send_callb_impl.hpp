#pragma once

#include <Callb_impl.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Send_callb_impl : public Callb_impl<Mqtt_port::O_callb>
        {
        public:
            Send_callb_impl(Grip &io_callb);

            void on_failure(const mqtt::token &asyncActionToken) override;

            // disabled, becaurse doesn't work
            void on_success(const mqtt::token &asyncActionToken) override;
        };
    }
}