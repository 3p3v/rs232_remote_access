#pragma once

#include <string>
#include <Callb.hpp>
#include <Rec_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        // class Rec_callb;
        
        template <typename Ok_callb, typename Ec_callb>
        class Unsub_callb final : public Callb<Ok_callb, Ec_callb>
        {
            using Rec_callb_cont = std::unordered_map<std::string, std::unique_ptr<Rec_callb_intf>>;
            Rec_callb_cont &rec_callb;
            std::string channel_name;

            void erase_callb();

        public:
            template <typename Str>
            Unsub_callb(Rec_callb_cont &rec_callb, Str &&channel_name, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            void on_success(const mqtt::token &asyncActionToken) override;
            void on_failure(const mqtt::token &asyncActionToken) override;
        };

        template <typename Ok_callb, typename Ec_callb>
        template <typename Str>
        Unsub_callb<Ok_callb, Ec_callb>::Unsub_callb(Rec_callb_cont &rec_callb, Str &&channel_name, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : Callb{std::move(ok_callb), std::move(ec_callb)}, rec_callb{rec_callb}, channel_name{std::forward<Str>(channel_name)}
        {
        }

        template <typename Ok_callb, typename Ec_callb>
        void Unsub_callb<Ok_callb, Ec_callb>::erase_callb()
        {
            rec_callb.erase(channel_name);
        }

        template <typename Ok_callb, typename Ec_callb>
        void Unsub_callb<Ok_callb, Ec_callb>::on_success(const mqtt::token &asyncActionToken)
        {
            erase_callb();
            ok_callb();
        }

        template <typename Ok_callb, typename Ec_callb>
        void Unsub_callb<Ok_callb, Ec_callb>::on_failure(const mqtt::token &asyncActionToken)
        {
            ec_callb(asyncActionToken.get_reason_code());
        }

        template <typename Str, typename Ok_callb, typename Ec_callb>
        decltype(auto) make_unsub_callb(std::unordered_map<std::string, std::unique_ptr<Rec_callb_intf>> &rec_callb, Str &&channel_name, Ok_callb &&ok_callb, Ec_callb&& ec_callb)
        {
            return std::make_unique<Unsub_callb<Ok_callb, Ec_callb>>(rec_callb, std::forward<Str>(channel_name), std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }
    }
}
