#pragma once

#include <impl/basic_callbacks/Callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Ok_callb, typename Ec_callb>
        class Vanilla_callb final : public Callb<Ok_callb, Ec_callb>
        {
        public:
            Vanilla_callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            void on_success(const mqtt::token &asyncActionToken) override;
        };

        template <typename Ok_callb, typename Ec_callb>
        Vanilla_callb<Ok_callb, Ec_callb>::Vanilla_callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : Callb{std::move(ok_callb), std::move(ec_callb)}
        {
        }

        template <typename Ok_callb, typename Ec_callb>
        void Vanilla_callb<Ok_callb, Ec_callb>::on_success(const mqtt::token &asyncActionToken)
        {
            ok_callb();
        }

        template <typename Ok_callb, typename Ec_callb>
        decltype(auto) make_vanilla_callb(Ok_callb &&ok_callb, Ec_callb&& ec_callb)
        {
            return std::make_unique<Vanilla_callb<Ok_callb, Ec_callb>>(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }
    }
}