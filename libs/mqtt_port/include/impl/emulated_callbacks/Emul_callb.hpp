#pragma once
#include <Basic_emul_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Ok_callb, typename Ec_callb, typename ...Args>
        class Emul_callb final : public Basic_emul_callb<Args...>
        {
            Ok_callb ok_callb_;
            Ec_callb ec_callb_;

        public:
            Emul_callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            void ok_callb(Args ...args) override;
            void ec_callb(int code) override;
        };

        template <typename Ok_callb, typename Ec_callb, typename ...Args>
        Emul_callb<Ok_callb, Ec_callb, Args...>::Emul_callb(Ok_callb &&ok_callb_, Ec_callb &&ec_callb_)
            : ok_callb_{std::move(ok_callb_)}, ec_callb_{std::move(ec_callb_)}
        {

        }

        template <typename Ok_callb, typename Ec_callb, typename ...Args>
        void Emul_callb<Ok_callb, Ec_callb, Args...>::ok_callb(Args ...args)
        {
            ok_callb_(args...);
        }

        template <typename Ok_callb, typename Ec_callb, typename ...Args>
        void Emul_callb<Ok_callb, Ec_callb, Args...>::ec_callb(int code)
        {
            ec_callb_(code);
        }
    }
}