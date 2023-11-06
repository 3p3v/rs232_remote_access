#pragma once

#include <string>
#include <IO_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Ok_callb, typename Ec_callb>
        class IO_callb_data final : public IO_callb<Ok_callb, Ec_callb>
        {
            using Data = std::string;
            std::shared_ptr<Data> data;

        public:
            IO_callb_data(std::shared_ptr<Data> data, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            // void on_success(const mqtt::token &asyncActionToken) override;
        };

        template <typename Ok_callb, typename Ec_callb>
        IO_callb_data<Ok_callb, Ec_callb>::IO_callb_data(std::shared_ptr<Data> data, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : IO_callb{len, std::move(ok_callb), std::move(ec_callb)}, data{std::move(data)}
        {
        }

        // template <typename Ok_callb, typename Ec_callb>
        // void IO_callb_data<Ok_callb, Ec_callb>::on_success(const mqtt::token &asyncActionToken)
        // {
        //     ok_callb(len);
        // }

        template <typename Ok_callb, typename Ec_callb>
        IO_callb_data(std::shared_ptr<Data>, size_t, Ok_callb &&, Ec_callb &&) -> IO_callb_data<Ok_callb, Ec_callb>;

    }
}