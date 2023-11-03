#pragma once

namespace Mqtt_port
{
    namespace Impl
    {
        /// @brief Basic callback, utilises two functors as callbacks
        /// @tparam Ok_callb Callback called when action was successful
        /// @tparam Ec_callb Callback called when action was NOT successful
        template <typename Ok_callb, typename Ec_callb>
        class Basic_callb
        {
        protected:
            Ok_callb ok_callb;
            Ec_callb ec_callb;

        public:
            Basic_callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb);
            Basic_callb(Basic_callb&&) = default;
            Basic_callb& operator=(Basic_callb&&) = default;
            Basic_callb(Basic_callb&) = default;
            Basic_callb& operator=(Basic_callb&) = default;
            virtual ~Basic_callb() = default;
        };

        template <typename Ok_callb, typename Ec_callb>
        Basic_callb<Ok_callb, Ec_callb>::Basic_callb(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
            : ok_callb{std::move(ok_callb)}, ec_callb{std::move(ec_callb)}
        {
        }

        template <typename Ok_callb, typename Ec_callb>
        Basic_callb(Ok_callb &&, Ec_callb &&) -> Basic_callb<Ok_callb, Ec_callb>;
    }
}