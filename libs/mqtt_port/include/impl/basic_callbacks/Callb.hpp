#pragma once

#include <Basic_callb.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Ok_callb, typename Ec_callb>
        class Callb : public Basic_callb<Ok_callb, Ec_callb>, public mqtt::iaction_listener
        {
        public:
            Callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            void on_failure(const mqtt::token &asyncActionToken) override final;

            void on_success(const mqtt::token &asyncActionToken) = 0;
        };

        template <typename Ok_callb, typename Ec_callb>
        Callb<Ok_callb, Ec_callb>::Callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : Basic_callb{std::move(ok_callb), std::move(ec_callb)}
        {
        }

        template <typename Ok_callb, typename Ec_callb>
        void Callb<Ok_callb, Ec_callb>::on_failure(const mqtt::token &asyncActionToken)
        {
            ec_callb(asyncActionToken.get_return_code());
        }
    }
}