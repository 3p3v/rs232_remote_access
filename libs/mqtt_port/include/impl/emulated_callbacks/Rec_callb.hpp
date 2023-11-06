#pragma once

#include <string>
#include <Emul_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        using Rec_callb_intf = Basic_emul_callb<const std::string::const_iterator, const std::string::const_iterator>;
        
        template <typename Ok_callb, typename Ec_callb>
        using Rec_callb = Emul_callb<Ok_callb, Ec_callb, const std::string::const_iterator, const std::string::const_iterator>;

        template <typename Ok_callb, typename Ec_callb>
        decltype(auto) make_rec_callb(Ok_callb &&ok_callb, Ec_callb&& ec_callb)
        {
            return std::make_unique<Rec_callb<Ok_callb, Ec_callb>>(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }
    }
}
