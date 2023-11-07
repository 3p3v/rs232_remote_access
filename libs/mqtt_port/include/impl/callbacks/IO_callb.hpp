#pragma once

#include <impl/basic_callbacks/Callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Ok_callb, typename Ec_callb>
        class IO_callb : public Callb<Ok_callb, Ec_callb>
        {
            size_t len;

        public:
            IO_callb(size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            void on_success(const mqtt::token &asyncActionToken) override;
        };

        template <typename Ok_callb, typename Ec_callb>
        IO_callb<Ok_callb, Ec_callb>::IO_callb(size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : Callb{std::move(ok_callb), std::move(ec_callb)}, len{len}
        {
        }

        template <typename Ok_callb, typename Ec_callb>
        void IO_callb<Ok_callb, Ec_callb>::on_success(const mqtt::token &asyncActionToken)
        {
            ok_callb(len);
        }

        template <typename Ok_callb, typename Ec_callb>
        IO_callb(size_t, Ok_callb &&, Ec_callb &&) -> IO_callb<Ok_callb, Ec_callb>;

        template <typename Ok_callb, typename Ec_callb>
        decltype(auto) make_io_callb(size_t len, Ok_callb &&ok_callb, Ec_callb&& ec_callb)
        {
            return std::make_unique<IO_callb<Ok_callb, Ec_callb>>(len, std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }
    }
}